/**************************************************************************

Copyright (c) 2007-2008, Chelsio Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.

 2. Neither the name of the Chelsio Corporation nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

***************************************************************************/

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/bus.h>
#include <sys/module.h>
#include <sys/pciio.h>
#include <sys/conf.h>
#include <machine/bus.h>
#include <machine/resource.h>
#include <sys/bus_dma.h>
#include <sys/ktr.h>
#include <sys/rman.h>
#include <sys/ioccom.h>
#include <sys/mbuf.h>
#include <sys/linker.h>
#include <sys/firmware.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <sys/smp.h>
#include <sys/sysctl.h>
#include <sys/syslog.h>
#include <sys/queue.h>
#include <sys/taskqueue.h>
#include <sys/proc.h>

#include <net/bpf.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <net/if_dl.h>
#include <net/if_media.h>
#include <net/if_types.h>
#include <net/if_vlan_var.h>

#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>

#include <dev/pci/pcireg.h>
#include <dev/pci/pcivar.h>
#include <dev/pci/pci_private.h>

#include <cxgb_include.h>

#ifdef PRIV_SUPPORTED
#include <sys/priv.h>
#endif

static int cxgb_setup_msix(adapter_t *, int);
static void cxgb_teardown_msix(adapter_t *);
static void cxgb_init(void *);
static void cxgb_init_locked(struct port_info *);
static void cxgb_stop_locked(struct port_info *);
static void cxgb_set_rxmode(struct port_info *);
static int cxgb_ioctl(struct ifnet *, unsigned long, caddr_t);
static int cxgb_media_change(struct ifnet *);
static int cxgb_ifm_type(int);
static void cxgb_media_status(struct ifnet *, struct ifmediareq *);
static int setup_sge_qsets(adapter_t *);
static void cxgb_async_intr(void *);
static void cxgb_ext_intr_handler(void *, int);
static void cxgb_tick_handler(void *, int);
static void cxgb_down_locked(struct adapter *sc);
static void cxgb_tick(void *);
static void setup_rss(adapter_t *sc);

/* Attachment glue for the PCI controller end of the device.  Each port of
 * the device is attached separately, as defined later.
 */
static int cxgb_controller_probe(device_t);
static int cxgb_controller_attach(device_t);
static int cxgb_controller_detach(device_t);
static void cxgb_free(struct adapter *);
static __inline void reg_block_dump(struct adapter *ap, uint8_t *buf, unsigned int start,
    unsigned int end);
static void cxgb_get_regs(adapter_t *sc, struct ch_ifconf_regs *regs, uint8_t *buf);
static int cxgb_get_regs_len(void);
static int offload_open(struct port_info *pi);
static void touch_bars(device_t dev);
static int offload_close(struct t3cdev *tdev);
static void cxgb_link_start(struct port_info *p);

static device_method_t cxgb_controller_methods[] = {
	DEVMETHOD(device_probe,		cxgb_controller_probe),
	DEVMETHOD(device_attach,	cxgb_controller_attach),
	DEVMETHOD(device_detach,	cxgb_controller_detach),

	/* bus interface */
	DEVMETHOD(bus_print_child,	bus_generic_print_child),
	DEVMETHOD(bus_driver_added,	bus_generic_driver_added),

	{ 0, 0 }
};

static driver_t cxgb_controller_driver = {
	"cxgbc",
	cxgb_controller_methods,
	sizeof(struct adapter)
};

static devclass_t	cxgb_controller_devclass;
DRIVER_MODULE(cxgbc, pci, cxgb_controller_driver, cxgb_controller_devclass, 0, 0);

/*
 * Attachment glue for the ports.  Attachment is done directly to the
 * controller device.
 */
static int cxgb_port_probe(device_t);
static int cxgb_port_attach(device_t);
static int cxgb_port_detach(device_t);

static device_method_t cxgb_port_methods[] = {
	DEVMETHOD(device_probe,		cxgb_port_probe),
	DEVMETHOD(device_attach,	cxgb_port_attach),
	DEVMETHOD(device_detach,	cxgb_port_detach),
	{ 0, 0 }
};

static driver_t cxgb_port_driver = {
	"cxgb",
	cxgb_port_methods,
	0
};

static d_ioctl_t cxgb_extension_ioctl;
static d_open_t cxgb_extension_open;
static d_close_t cxgb_extension_close;

static struct cdevsw cxgb_cdevsw = {
       .d_version =    D_VERSION,
       .d_flags =      0,
       .d_open =       cxgb_extension_open,
       .d_close =      cxgb_extension_close,
       .d_ioctl =      cxgb_extension_ioctl,
       .d_name =       "cxgb",
};

static devclass_t	cxgb_port_devclass;
DRIVER_MODULE(cxgb, cxgbc, cxgb_port_driver, cxgb_port_devclass, 0, 0);

#define SGE_MSIX_COUNT (SGE_QSETS + 1)

/*
 * The driver uses the best interrupt scheme available on a platform in the
 * order MSI-X, MSI, legacy pin interrupts.  This parameter determines which
 * of these schemes the driver may consider as follows:
 *
 * msi = 2: choose from among all three options
 * msi = 1 : only consider MSI and pin interrupts
 * msi = 0: force pin interrupts
 */
static int msi_allowed = 2;

TUNABLE_INT("hw.cxgb.msi_allowed", &msi_allowed);
SYSCTL_NODE(_hw, OID_AUTO, cxgb, CTLFLAG_RD, 0, "CXGB driver parameters");
SYSCTL_UINT(_hw_cxgb, OID_AUTO, msi_allowed, CTLFLAG_RDTUN, &msi_allowed, 0,
    "MSI-X, MSI, INTx selector");

/*
 * The driver enables offload as a default.
 * To disable it, use ofld_disable = 1.
 */
static int ofld_disable = 0;
TUNABLE_INT("hw.cxgb.ofld_disable", &ofld_disable);
SYSCTL_UINT(_hw_cxgb, OID_AUTO, ofld_disable, CTLFLAG_RDTUN, &ofld_disable, 0,
    "disable ULP offload");

/*
 * The driver uses an auto-queue algorithm by default.
 * To disable it and force a single queue-set per port, use multiq = 0
 */
static int multiq = 1;
TUNABLE_INT("hw.cxgb.multiq", &multiq);
SYSCTL_UINT(_hw_cxgb, OID_AUTO, multiq, CTLFLAG_RDTUN, &multiq, 0,
    "use min(ncpus/ports, 8) queue-sets per port");

/*
 * By default the driver will not update the firmware unless
 * it was compiled against a newer version
 * 
 */
static int force_fw_update = 0;
TUNABLE_INT("hw.cxgb.force_fw_update", &force_fw_update);
SYSCTL_UINT(_hw_cxgb, OID_AUTO, force_fw_update, CTLFLAG_RDTUN, &force_fw_update, 0,
    "update firmware even if up to date");

int cxgb_use_16k_clusters = 1;
TUNABLE_INT("hw.cxgb.use_16k_clusters", &cxgb_use_16k_clusters);
SYSCTL_UINT(_hw_cxgb, OID_AUTO, use_16k_clusters, CTLFLAG_RDTUN,
    &cxgb_use_16k_clusters, 0, "use 16kB clusters for the jumbo queue ");

enum {
	MAX_TXQ_ENTRIES      = 16384,
	MAX_CTRL_TXQ_ENTRIES = 1024,
	MAX_RSPQ_ENTRIES     = 16384,
	MAX_RX_BUFFERS       = 16384,
	MAX_RX_JUMBO_BUFFERS = 16384,
	MIN_TXQ_ENTRIES      = 4,
	MIN_CTRL_TXQ_ENTRIES = 4,
	MIN_RSPQ_ENTRIES     = 32,
	MIN_FL_ENTRIES       = 32,
	MIN_FL_JUMBO_ENTRIES = 32
};

struct filter_info {
	u32 sip;
	u32 sip_mask;
	u32 dip;
	u16 sport;
	u16 dport;
	u32 vlan:12;
	u32 vlan_prio:3;
	u32 mac_hit:1;
	u32 mac_idx:4;
	u32 mac_vld:1;
	u32 pkt_type:2;
	u32 report_filter_id:1;
	u32 pass:1;
	u32 rss:1;
	u32 qset:3;
	u32 locked:1;
	u32 valid:1;
};

enum { FILTER_NO_VLAN_PRI = 7 };

#define EEPROM_MAGIC 0x38E2F10C

#define PORT_MASK ((1 << MAX_NPORTS) - 1)

/* Table for probing the cards.  The desc field isn't actually used */
struct cxgb_ident {
	uint16_t	vendor;
	uint16_t	device;
	int		index;
	char		*desc;
} cxgb_identifiers[] = {
	{PCI_VENDOR_ID_CHELSIO, 0x0020, 0, "PE9000"},
	{PCI_VENDOR_ID_CHELSIO, 0x0021, 1, "T302E"},
	{PCI_VENDOR_ID_CHELSIO, 0x0022, 2, "T310E"},
	{PCI_VENDOR_ID_CHELSIO, 0x0023, 3, "T320X"},
	{PCI_VENDOR_ID_CHELSIO, 0x0024, 1, "T302X"},
	{PCI_VENDOR_ID_CHELSIO, 0x0025, 3, "T320E"},
	{PCI_VENDOR_ID_CHELSIO, 0x0026, 2, "T310X"},
	{PCI_VENDOR_ID_CHELSIO, 0x0030, 2, "T3B10"},
	{PCI_VENDOR_ID_CHELSIO, 0x0031, 3, "T3B20"},
	{PCI_VENDOR_ID_CHELSIO, 0x0032, 1, "T3B02"},
	{PCI_VENDOR_ID_CHELSIO, 0x0033, 4, "T3B04"},
	{PCI_VENDOR_ID_CHELSIO, 0x0035, 6, "N310E"},
	{0, 0, 0, NULL}
};

static int set_eeprom(struct port_info *pi, const uint8_t *data, int len, int offset);


static __inline char
t3rev2char(struct adapter *adapter)
{
	char rev = 'z';

	switch(adapter->params.rev) {
	case T3_REV_A:
		rev = 'a';
		break;
	case T3_REV_B:
	case T3_REV_B2:
		rev = 'b';
		break;
	case T3_REV_C:
		rev = 'c';
		break;
	}
	return rev;
}

static struct cxgb_ident *
cxgb_get_ident(device_t dev)
{
	struct cxgb_ident *id;

	for (id = cxgb_identifiers; id->desc != NULL; id++) {
		if ((id->vendor == pci_get_vendor(dev)) &&
		    (id->device == pci_get_device(dev))) {
			return (id);
		}
	}
	return (NULL);
}

static const struct adapter_info *
cxgb_get_adapter_info(device_t dev)
{
	struct cxgb_ident *id;
	const struct adapter_info *ai;

	id = cxgb_get_ident(dev);
	if (id == NULL)
		return (NULL);

	ai = t3_get_adapter_info(id->index);

	return (ai);
}

static int
cxgb_controller_probe(device_t dev)
{
	const struct adapter_info *ai;
	char *ports, buf[80];
	int nports;
	struct adapter *sc = device_get_softc(dev);

	ai = cxgb_get_adapter_info(dev);
	if (ai == NULL)
		return (ENXIO);

	nports = ai->nports0 + ai->nports1;
	if (nports == 1)
		ports = "port";
	else
		ports = "ports";

	snprintf(buf, sizeof(buf), "%s %sNIC, rev: %d nports: %d %s",
	    ai->desc, is_offload(sc) ? "R" : "",
	    sc->params.rev, nports, ports);
	device_set_desc_copy(dev, buf);
	return (BUS_PROBE_DEFAULT);
}

#define FW_FNAME "cxgb_t3fw"
#define TPEEPROM_NAME "t3b_tp_eeprom"
#define TPSRAM_NAME "t3b_protocol_sram"

static int
upgrade_fw(adapter_t *sc)
{
#ifdef FIRMWARE_LATEST
	const struct firmware *fw;
#else
	struct firmware *fw;
#endif	
	int status;
	
	if ((fw = firmware_get(FW_FNAME)) == NULL)  {
		device_printf(sc->dev, "Could not find firmware image %s\n", FW_FNAME);
		return (ENOENT);
	} else
		device_printf(sc->dev, "updating firmware on card\n");
	status = t3_load_fw(sc, (const uint8_t *)fw->data, fw->datasize);

	device_printf(sc->dev, "firmware update returned %s %d\n", (status == 0) ? "success" : "fail", status);
	
	firmware_put(fw, FIRMWARE_UNLOAD);

	return (status);	
}

static int
cxgb_controller_attach(device_t dev)
{
	device_t child;
	const struct adapter_info *ai;
	struct adapter *sc;
	int i, error = 0;
	uint32_t vers;
	int port_qsets = 1;
#ifdef MSI_SUPPORTED
	int msi_needed, reg;
#endif
	int must_load = 0;
	char buf[80];

	sc = device_get_softc(dev);
	sc->dev = dev;
	sc->msi_count = 0;
	ai = cxgb_get_adapter_info(dev);

	/*
	 * XXX not really related but a recent addition
	 */
#ifdef MSI_SUPPORTED	
	/* find the PCIe link width and set max read request to 4KB*/
	if (pci_find_extcap(dev, PCIY_EXPRESS, &reg) == 0) {
		uint16_t lnk, pectl;
		lnk = pci_read_config(dev, reg + 0x12, 2);
		sc->link_width = (lnk >> 4) & 0x3f;
		
		pectl = pci_read_config(dev, reg + 0x8, 2);
		pectl = (pectl & ~0x7000) | (5 << 12);
		pci_write_config(dev, reg + 0x8, pectl, 2);
	}

	if (sc->link_width != 0 && sc->link_width <= 4 &&
	    (ai->nports0 + ai->nports1) <= 2) {
		device_printf(sc->dev,
		    "PCIe x%d Link, expect reduced performance\n",
		    sc->link_width);
	}
#endif
	touch_bars(dev);
	pci_enable_busmaster(dev);
	/*
	 * Allocate the registers and make them available to the driver.
	 * The registers that we care about for NIC mode are in BAR 0
	 */
	sc->regs_rid = PCIR_BAR(0);
	if ((sc->regs_res = bus_alloc_resource_any(dev, SYS_RES_MEMORY,
	    &sc->regs_rid, RF_ACTIVE)) == NULL) {
		device_printf(dev, "Cannot allocate BAR region 0\n");
		return (ENXIO);
	}
	sc->udbs_rid = PCIR_BAR(2);
	sc->udbs_res = NULL;
	if (is_offload(sc) &&
	    ((sc->udbs_res = bus_alloc_resource_any(dev, SYS_RES_MEMORY,
		   &sc->udbs_rid, RF_ACTIVE)) == NULL)) {
		device_printf(dev, "Cannot allocate BAR region 1\n");
		error = ENXIO;
		goto out;
	}

	snprintf(sc->lockbuf, ADAPTER_LOCK_NAME_LEN, "cxgb controller lock %d",
	    device_get_unit(dev));
	ADAPTER_LOCK_INIT(sc, sc->lockbuf);

	snprintf(sc->reglockbuf, ADAPTER_LOCK_NAME_LEN, "SGE reg lock %d",
	    device_get_unit(dev));
	snprintf(sc->mdiolockbuf, ADAPTER_LOCK_NAME_LEN, "cxgb mdio lock %d",
	    device_get_unit(dev));
	snprintf(sc->elmerlockbuf, ADAPTER_LOCK_NAME_LEN, "cxgb elmer lock %d",
	    device_get_unit(dev));
	
	MTX_INIT(&sc->sge.reg_lock, sc->reglockbuf, NULL, MTX_SPIN);
	MTX_INIT(&sc->mdio_lock, sc->mdiolockbuf, NULL, MTX_DEF);
	MTX_INIT(&sc->elmer_lock, sc->elmerlockbuf, NULL, MTX_DEF);
	
	sc->bt = rman_get_bustag(sc->regs_res);
	sc->bh = rman_get_bushandle(sc->regs_res);
	sc->mmio_len = rman_get_size(sc->regs_res);

	if (t3_prep_adapter(sc, ai, 1) < 0) {
		printf("prep adapter failed\n");
		error = ENODEV;
		goto out;
	}
        /* Allocate the BAR for doing MSI-X.  If it succeeds, try to allocate
	 * enough messages for the queue sets.  If that fails, try falling
	 * back to MSI.  If that fails, then try falling back to the legacy
	 * interrupt pin model.
	 */
#ifdef MSI_SUPPORTED

	sc->msix_regs_rid = 0x20;
	if ((msi_allowed >= 2) &&
	    (sc->msix_regs_res = bus_alloc_resource_any(dev, SYS_RES_MEMORY,
	    &sc->msix_regs_rid, RF_ACTIVE)) != NULL) {

		msi_needed = sc->msi_count = SGE_MSIX_COUNT;

		if (((error = pci_alloc_msix(dev, &sc->msi_count)) != 0) ||
		    (sc->msi_count != msi_needed)) {
			device_printf(dev, "msix allocation failed - msi_count = %d"
			    " msi_needed=%d will try msi err=%d\n", sc->msi_count,
			    msi_needed, error);
			sc->msi_count = 0;
			pci_release_msi(dev);
			bus_release_resource(dev, SYS_RES_MEMORY,
			    sc->msix_regs_rid, sc->msix_regs_res);
			sc->msix_regs_res = NULL;
		} else {
			sc->flags |= USING_MSIX;
			sc->cxgb_intr = t3_intr_msix;
		}
	}

	if ((msi_allowed >= 1) && (sc->msi_count == 0)) {
		sc->msi_count = 1;
		if (pci_alloc_msi(dev, &sc->msi_count)) {
			device_printf(dev, "alloc msi failed - will try INTx\n");
			sc->msi_count = 0;
			pci_release_msi(dev);
		} else {
			sc->flags |= USING_MSI;
			sc->irq_rid = 1;
			sc->cxgb_intr = t3_intr_msi;
		}
	}
#endif
	if (sc->msi_count == 0) {
		device_printf(dev, "using line interrupts\n");
		sc->irq_rid = 0;
		sc->cxgb_intr = t3b_intr;
	}

	if ((sc->flags & USING_MSIX) && multiq)
		port_qsets = min((SGE_QSETS/(sc)->params.nports), mp_ncpus);
	
	/* Create a private taskqueue thread for handling driver events */
#ifdef TASKQUEUE_CURRENT	
	sc->tq = taskqueue_create("cxgb_taskq", M_NOWAIT,
	    taskqueue_thread_enqueue, &sc->tq);
#else
	sc->tq = taskqueue_create_fast("cxgb_taskq", M_NOWAIT,
	    taskqueue_thread_enqueue, &sc->tq);
#endif	
	if (sc->tq == NULL) {
		device_printf(dev, "failed to allocate controller task queue\n");
		goto out;
	}

	taskqueue_start_threads(&sc->tq, 1, PI_NET, "%s taskq",
	    device_get_nameunit(dev));
	TASK_INIT(&sc->ext_intr_task, 0, cxgb_ext_intr_handler, sc);
	TASK_INIT(&sc->tick_task, 0, cxgb_tick_handler, sc);

	
	/* Create a periodic callout for checking adapter status */
	callout_init(&sc->cxgb_tick_ch, TRUE);
	
	if ((t3_check_fw_version(sc, &must_load) != 0 && must_load) || force_fw_update) {
		/*
		 * Warn user that a firmware update will be attempted in init.
		 */
		device_printf(dev, "firmware needs to be updated to version %d.%d.%d\n",
		    FW_VERSION_MAJOR, FW_VERSION_MINOR, FW_VERSION_MICRO);
		sc->flags &= ~FW_UPTODATE;
	} else {
		sc->flags |= FW_UPTODATE;
	}

	if (t3_check_tpsram_version(sc, &must_load) != 0 && must_load) {
		/*
		 * Warn user that a firmware update will be attempted in init.
		 */
		device_printf(dev, "SRAM needs to be updated to version %c-%d.%d.%d\n",
		    t3rev2char(sc), TP_VERSION_MAJOR, TP_VERSION_MINOR, TP_VERSION_MICRO);
		sc->flags &= ~TPS_UPTODATE;
	} else {
		sc->flags |= TPS_UPTODATE;
	}
	
	/*
	 * Create a child device for each MAC.  The ethernet attachment
	 * will be done in these children.
	 */	
	for (i = 0; i < (sc)->params.nports; i++) {
		struct port_info *pi;
		
		if ((child = device_add_child(dev, "cxgb", -1)) == NULL) {
			device_printf(dev, "failed to add child port\n");
			error = EINVAL;
			goto out;
		}
		pi = &sc->port[i];
		pi->adapter = sc;
		pi->nqsets = port_qsets;
		pi->first_qset = i*port_qsets;
		pi->port_id = i;
		pi->tx_chan = i >= ai->nports0;
		pi->txpkt_intf = pi->tx_chan ? 2 * (i - ai->nports0) + 1 : 2 * i;
		sc->rxpkt_map[pi->txpkt_intf] = i;
		sc->port[i].tx_chan = i >= ai->nports0;
		sc->portdev[i] = child;
		device_set_softc(child, pi);
	}
	if ((error = bus_generic_attach(dev)) != 0)
		goto out;

	/* initialize sge private state */
	t3_sge_init_adapter(sc);

	t3_led_ready(sc);
	
	cxgb_offload_init();
	if (is_offload(sc)) {
		setbit(&sc->registered_device_map, OFFLOAD_DEVMAP_BIT);
		cxgb_adapter_ofld(sc);
        }
	error = t3_get_fw_version(sc, &vers);
	if (error)
		goto out;

	snprintf(&sc->fw_version[0], sizeof(sc->fw_version), "%d.%d.%d",
	    G_FW_VERSION_MAJOR(vers), G_FW_VERSION_MINOR(vers),
	    G_FW_VERSION_MICRO(vers));

	snprintf(buf, sizeof(buf), "%s\t E/C: %s S/N: %s", 
		 ai->desc,
		 sc->params.vpd.ec, sc->params.vpd.sn);
	device_set_desc_copy(dev, buf);

	device_printf(sc->dev, "Firmware Version %s\n", &sc->fw_version[0]);
	callout_reset(&sc->cxgb_tick_ch, CXGB_TICKS(sc), cxgb_tick, sc);
	t3_add_attach_sysctls(sc);
out:
	if (error)
		cxgb_free(sc);

	return (error);
}

static int
cxgb_controller_detach(device_t dev)
{
	struct adapter *sc;

	sc = device_get_softc(dev);

	cxgb_free(sc);

	return (0);
}

static void
cxgb_free(struct adapter *sc)
{
	int i;

	ADAPTER_LOCK(sc);
	sc->flags |= CXGB_SHUTDOWN;
	ADAPTER_UNLOCK(sc);
	cxgb_pcpu_shutdown_threads(sc);
	ADAPTER_LOCK(sc);

/*
 * drops the lock
 */
	cxgb_down_locked(sc);
	
#ifdef MSI_SUPPORTED
	if (sc->flags & (USING_MSI | USING_MSIX)) {
		device_printf(sc->dev, "releasing msi message(s)\n");
		pci_release_msi(sc->dev);
	} else {
		device_printf(sc->dev, "no msi message to release\n");
	}
#endif
	if (sc->msix_regs_res != NULL) {
		bus_release_resource(sc->dev, SYS_RES_MEMORY, sc->msix_regs_rid,
		    sc->msix_regs_res);
	}

	t3_sge_deinit_sw(sc);
	/*
	 * Wait for last callout
	 */
	
	DELAY(hz*100);

	for (i = 0; i < (sc)->params.nports; ++i) {
		if (sc->portdev[i] != NULL)
			device_delete_child(sc->dev, sc->portdev[i]);
	}
		
	bus_generic_detach(sc->dev);
	if (sc->tq != NULL) {
		taskqueue_free(sc->tq);
		sc->tq = NULL;
	}
	
	if (is_offload(sc)) {
		cxgb_adapter_unofld(sc);
		if (isset(&sc->open_device_map,	OFFLOAD_DEVMAP_BIT))
			offload_close(&sc->tdev);
		else
			printf("cxgb_free: DEVMAP_BIT not set\n");
	} else
		printf("not offloading set\n");	
#ifdef notyet
	if (sc->flags & CXGB_OFLD_INIT)
		cxgb_offload_deactivate(sc);
#endif
	free(sc->filters, M_DEVBUF);
	t3_sge_free(sc);
	
	cxgb_offload_exit();

	if (sc->udbs_res != NULL)
		bus_release_resource(sc->dev, SYS_RES_MEMORY, sc->udbs_rid,
		    sc->udbs_res);

	if (sc->regs_res != NULL)
		bus_release_resource(sc->dev, SYS_RES_MEMORY, sc->regs_rid,
		    sc->regs_res);

	MTX_DESTROY(&sc->mdio_lock);
	MTX_DESTROY(&sc->sge.reg_lock);
	MTX_DESTROY(&sc->elmer_lock);
	ADAPTER_LOCK_DEINIT(sc);
}

/**
 *	setup_sge_qsets - configure SGE Tx/Rx/response queues
 *	@sc: the controller softc
 *
 *	Determines how many sets of SGE queues to use and initializes them.
 *	We support multiple queue sets per port if we have MSI-X, otherwise
 *	just one queue set per port.
 */
static int
setup_sge_qsets(adapter_t *sc)
{
	int i, j, err, irq_idx = 0, qset_idx = 0;
	u_int ntxq = SGE_TXQ_PER_SET;

	if ((err = t3_sge_alloc(sc)) != 0) {
		device_printf(sc->dev, "t3_sge_alloc returned %d\n", err);
		return (err);
	}

	if (sc->params.rev > 0 && !(sc->flags & USING_MSI))
		irq_idx = -1;

	for (i = 0; i < (sc)->params.nports; i++) {
		struct port_info *pi = &sc->port[i];

		for (j = 0; j < pi->nqsets; j++, qset_idx++) {
			err = t3_sge_alloc_qset(sc, qset_idx, (sc)->params.nports,
			    (sc->flags & USING_MSIX) ? qset_idx + 1 : irq_idx,
			    &sc->params.sge.qset[qset_idx], ntxq, pi);
			if (err) {
				t3_free_sge_resources(sc);
				device_printf(sc->dev, "t3_sge_alloc_qset failed with %d\n",
				    err);
				return (err);
			}
		}
	}

	return (0);
}

static void
cxgb_teardown_msix(adapter_t *sc) 
{
	int i, nqsets;
	
	for (nqsets = i = 0; i < (sc)->params.nports; i++) 
		nqsets += sc->port[i].nqsets;

	for (i = 0; i < nqsets; i++) {
		if (sc->msix_intr_tag[i] != NULL) {
			bus_teardown_intr(sc->dev, sc->msix_irq_res[i],
			    sc->msix_intr_tag[i]);
			sc->msix_intr_tag[i] = NULL;
		}
		if (sc->msix_irq_res[i] != NULL) {
			bus_release_resource(sc->dev, SYS_RES_IRQ,
			    sc->msix_irq_rid[i], sc->msix_irq_res[i]);
			sc->msix_irq_res[i] = NULL;
		}
	}
}

static int
cxgb_setup_msix(adapter_t *sc, int msix_count)
{
	int i, j, k, nqsets, rid;

	/* The first message indicates link changes and error conditions */
	sc->irq_rid = 1;
	if ((sc->irq_res = bus_alloc_resource_any(sc->dev, SYS_RES_IRQ,
	   &sc->irq_rid, RF_SHAREABLE | RF_ACTIVE)) == NULL) {
		device_printf(sc->dev, "Cannot allocate msix interrupt\n");
		return (EINVAL);
	}

	if (bus_setup_intr(sc->dev, sc->irq_res, INTR_MPSAFE|INTR_TYPE_NET,
#ifdef INTR_FILTERS
		NULL,
#endif
		cxgb_async_intr, sc, &sc->intr_tag)) {
		device_printf(sc->dev, "Cannot set up interrupt\n");
		return (EINVAL);
	}
	for (i = k = 0; i < (sc)->params.nports; i++) {
		nqsets = sc->port[i].nqsets;
		for (j = 0; j < nqsets; j++, k++) {
			struct sge_qset *qs = &sc->sge.qs[k];

			rid = k + 2;
			if (cxgb_debug)
				printf("rid=%d ", rid);
			if ((sc->msix_irq_res[k] = bus_alloc_resource_any(
			    sc->dev, SYS_RES_IRQ, &rid,
			    RF_SHAREABLE | RF_ACTIVE)) == NULL) {
				device_printf(sc->dev, "Cannot allocate "
				    "interrupt for message %d\n", rid);
				return (EINVAL);
			}
			sc->msix_irq_rid[k] = rid;
			if (bus_setup_intr(sc->dev, sc->msix_irq_res[k],
				INTR_MPSAFE|INTR_TYPE_NET,
#ifdef INTR_FILTERS
				NULL,
#endif
				t3_intr_msix, qs, &sc->msix_intr_tag[k])) {
				device_printf(sc->dev, "Cannot set up "
				    "interrupt for message %d\n", rid);
				return (EINVAL);
				
			}
#if 0			
#ifdef IFNET_MULTIQUEUE			
			if (multiq) {
				int vector = rman_get_start(sc->msix_irq_res[k]);
				if (bootverbose)
					device_printf(sc->dev, "binding vector=%d to cpu=%d\n", vector, k % mp_ncpus);
				intr_bind(vector, k % mp_ncpus);
			}
#endif
#endif
		}
	}

	return (0);
}

static int
cxgb_port_probe(device_t dev)
{
	struct port_info *p;
	char buf[80];
	const char *desc;
	
	p = device_get_softc(dev);
	desc = p->phy.desc;
	snprintf(buf, sizeof(buf), "Port %d %s", p->port_id, desc);
	device_set_desc_copy(dev, buf);
	return (0);
}


static int
cxgb_makedev(struct port_info *pi)
{
	
	pi->port_cdev = make_dev(&cxgb_cdevsw, pi->ifp->if_dunit,
	    UID_ROOT, GID_WHEEL, 0600, if_name(pi->ifp));
	
	if (pi->port_cdev == NULL)
		return (ENOMEM);

	pi->port_cdev->si_drv1 = (void *)pi;
	
	return (0);
}

#ifndef LRO_SUPPORTED
#ifdef IFCAP_LRO
#undef IFCAP_LRO
#endif
#define IFCAP_LRO 0x0
#endif

#ifdef TSO_SUPPORTED
#define CXGB_CAP (IFCAP_VLAN_HWTAGGING | IFCAP_VLAN_MTU | IFCAP_HWCSUM | IFCAP_VLAN_HWCSUM | IFCAP_TSO | IFCAP_JUMBO_MTU | IFCAP_LRO)
/* Don't enable TSO6 yet */
#define CXGB_CAP_ENABLE (IFCAP_VLAN_HWTAGGING | IFCAP_VLAN_MTU | IFCAP_HWCSUM | IFCAP_VLAN_HWCSUM | IFCAP_TSO4 | IFCAP_JUMBO_MTU | IFCAP_LRO)
#else
#define CXGB_CAP (IFCAP_VLAN_HWTAGGING | IFCAP_VLAN_MTU | IFCAP_HWCSUM | IFCAP_JUMBO_MTU)
/* Don't enable TSO6 yet */
#define CXGB_CAP_ENABLE (IFCAP_VLAN_HWTAGGING | IFCAP_VLAN_MTU | IFCAP_HWCSUM |  IFCAP_JUMBO_MTU)
#define IFCAP_TSO4 0x0
#define IFCAP_TSO6 0x0
#define CSUM_TSO   0x0
#endif


static int
cxgb_port_attach(device_t dev)
{
	struct port_info *p;
	struct ifnet *ifp;
	int err, media_flags;
	struct adapter *sc;
	
	
	p = device_get_softc(dev);
	sc = p->adapter;
	snprintf(p->lockbuf, PORT_NAME_LEN, "cxgb port lock %d:%d",
	    device_get_unit(device_get_parent(dev)), p->port_id);
	PORT_LOCK_INIT(p, p->lockbuf);

	/* Allocate an ifnet object and set it up */
	ifp = p->ifp = if_alloc(IFT_ETHER);
	if (ifp == NULL) {
		device_printf(dev, "Cannot allocate ifnet\n");
		return (ENOMEM);
	}
	
	/*
	 * Note that there is currently no watchdog timer.
	 */
	if_initname(ifp, device_get_name(dev), device_get_unit(dev));
	ifp->if_init = cxgb_init;
	ifp->if_softc = p;
	ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX | IFF_MULTICAST;
	ifp->if_ioctl = cxgb_ioctl;
	ifp->if_start = cxgb_start;


	ifp->if_timer = 0;	/* Disable ifnet watchdog */
	ifp->if_watchdog = NULL;

	ifp->if_snd.ifq_drv_maxlen = IFQ_MAXLEN;
	IFQ_SET_MAXLEN(&ifp->if_snd, ifp->if_snd.ifq_drv_maxlen);
	IFQ_SET_READY(&ifp->if_snd);

	ifp->if_hwassist = ifp->if_capabilities = ifp->if_capenable = 0;
	ifp->if_capabilities |= CXGB_CAP;
	ifp->if_capenable |= CXGB_CAP_ENABLE;
	ifp->if_hwassist |= (CSUM_TCP | CSUM_UDP | CSUM_IP | CSUM_TSO);
	/*
	 * disable TSO on 4-port - it isn't supported by the firmware yet
	 */	
	if (p->adapter->params.nports > 2) {
		ifp->if_capabilities &= ~(IFCAP_TSO4 | IFCAP_TSO6);
		ifp->if_capenable &= ~(IFCAP_TSO4 | IFCAP_TSO6);
		ifp->if_hwassist &= ~CSUM_TSO;
	}

	ether_ifattach(ifp, p->hw_addr);
#ifdef IFNET_MULTIQUEUE
	ifp->if_transmit = cxgb_pcpu_transmit;
#endif
	/*
	 * Only default to jumbo frames on 10GigE
	 */
	if (p->adapter->params.nports <= 2)
		ifp->if_mtu = ETHERMTU_JUMBO;
	if ((err = cxgb_makedev(p)) != 0) {
		printf("makedev failed %d\n", err);
		return (err);
	}
	ifmedia_init(&p->media, IFM_IMASK, cxgb_media_change,
	    cxgb_media_status);
      
	if (!strcmp(p->phy.desc,	"10GBASE-CX4")) {
		media_flags = IFM_ETHER | IFM_10G_CX4 | IFM_FDX;
	} else if (!strcmp(p->phy.desc, "10GBASE-SR")) {
		media_flags = IFM_ETHER | IFM_10G_SR | IFM_FDX;
	} else if (!strcmp(p->phy.desc, "10GBASE-R")) {
		media_flags = cxgb_ifm_type(p->phy.modtype);
	} else if (!strcmp(p->phy.desc, "10/100/1000BASE-T")) {
		ifmedia_add(&p->media, IFM_ETHER | IFM_10_T, 0, NULL);
		ifmedia_add(&p->media, IFM_ETHER | IFM_10_T | IFM_FDX,
			    0, NULL);
		ifmedia_add(&p->media, IFM_ETHER | IFM_100_TX,
			    0, NULL);
		ifmedia_add(&p->media, IFM_ETHER | IFM_100_TX | IFM_FDX,
			    0, NULL);
		ifmedia_add(&p->media, IFM_ETHER | IFM_1000_T | IFM_FDX,
			    0, NULL);
		media_flags = 0;
	} else if (!strcmp(p->phy.desc, "1000BASE-X")) {
		/*
		 * XXX: This is not very accurate.  Fix when common code
		 * returns more specific value - eg 1000BASE-SX, LX, etc.
		 *
		 * XXX: In the meantime, don't lie. Consider setting IFM_AUTO
		 * instead of SX.
		 */
		media_flags = IFM_ETHER | IFM_1000_SX | IFM_FDX;
	} else {
	        printf("unsupported media type %s\n", p->phy.desc);
		return (ENXIO);
	}
	if (media_flags) {
		/*
		 * Note the modtype on which we based our flags.  If modtype
		 * changes, we'll redo the ifmedia for this ifp.  modtype may
		 * change when transceivers are plugged in/out, and in other
		 * situations.
		 */
		ifmedia_add(&p->media, media_flags, p->phy.modtype, NULL);
		ifmedia_set(&p->media, media_flags);
	} else {
		ifmedia_add(&p->media, IFM_ETHER | IFM_AUTO, 0, NULL);
		ifmedia_set(&p->media, IFM_ETHER | IFM_AUTO);
	}	

	/* Get the latest mac address, User can use a LAA */
	bcopy(IF_LLADDR(p->ifp), p->hw_addr, ETHER_ADDR_LEN);
	t3_sge_init_port(p);
#if defined(LINK_ATTACH)	
	cxgb_link_start(p);
	t3_link_changed(sc, p->port_id);
#endif
	return (0);
}

static int
cxgb_port_detach(device_t dev)
{
	struct port_info *p;

	p = device_get_softc(dev);

	PORT_LOCK(p);
	if (p->ifp->if_drv_flags & IFF_DRV_RUNNING) 
		cxgb_stop_locked(p);
	PORT_UNLOCK(p);
	
	ether_ifdetach(p->ifp);
	printf("waiting for callout to stop ...");
	DELAY(1000000);
	printf("done\n");
	/*
	 * the lock may be acquired in ifdetach
	 */
	PORT_LOCK_DEINIT(p);
	if_free(p->ifp);
	
	if (p->port_cdev != NULL)
		destroy_dev(p->port_cdev);
	
	return (0);
}

void
t3_fatal_err(struct adapter *sc)
{
	u_int fw_status[4];

	if (sc->flags & FULL_INIT_DONE) {
		t3_sge_stop(sc);
		t3_write_reg(sc, A_XGM_TX_CTRL, 0);
		t3_write_reg(sc, A_XGM_RX_CTRL, 0);
		t3_write_reg(sc, XGM_REG(A_XGM_TX_CTRL, 1), 0);
		t3_write_reg(sc, XGM_REG(A_XGM_RX_CTRL, 1), 0);
		t3_intr_disable(sc);
	}
	device_printf(sc->dev,"encountered fatal error, operation suspended\n");
	if (!t3_cim_ctl_blk_read(sc, 0xa0, 4, fw_status))
		device_printf(sc->dev, "FW_ status: 0x%x, 0x%x, 0x%x, 0x%x\n",
		    fw_status[0], fw_status[1], fw_status[2], fw_status[3]);
}

int
t3_os_find_pci_capability(adapter_t *sc, int cap)
{
	device_t dev;
	struct pci_devinfo *dinfo;
	pcicfgregs *cfg;
	uint32_t status;
	uint8_t ptr;

	dev = sc->dev;
	dinfo = device_get_ivars(dev);
	cfg = &dinfo->cfg;

	status = pci_read_config(dev, PCIR_STATUS, 2);
	if (!(status & PCIM_STATUS_CAPPRESENT))
		return (0);

	switch (cfg->hdrtype & PCIM_HDRTYPE) {
	case 0:
	case 1:
		ptr = PCIR_CAP_PTR;
		break;
	case 2:
		ptr = PCIR_CAP_PTR_2;
		break;
	default:
		return (0);
		break;
	}
	ptr = pci_read_config(dev, ptr, 1);

	while (ptr != 0) {
		if (pci_read_config(dev, ptr + PCICAP_ID, 1) == cap)
			return (ptr);
		ptr = pci_read_config(dev, ptr + PCICAP_NEXTPTR, 1);
	}

	return (0);
}

int
t3_os_pci_save_state(struct adapter *sc)
{
	device_t dev;
	struct pci_devinfo *dinfo;

	dev = sc->dev;
	dinfo = device_get_ivars(dev);

	pci_cfg_save(dev, dinfo, 0);
	return (0);
}

int
t3_os_pci_restore_state(struct adapter *sc)
{
	device_t dev;
	struct pci_devinfo *dinfo;

	dev = sc->dev;
	dinfo = device_get_ivars(dev);

	pci_cfg_restore(dev, dinfo);
	return (0);
}

/**
 *	t3_os_link_changed - handle link status changes
 *	@adapter: the adapter associated with the link change
 *	@port_id: the port index whose limk status has changed
 *	@link_status: the new status of the link
 *	@speed: the new speed setting
 *	@duplex: the new duplex setting
 *	@fc: the new flow-control setting
 *
 *	This is the OS-dependent handler for link status changes.  The OS
 *	neutral handler takes care of most of the processing for these events,
 *	then calls this handler for any OS-specific processing.
 */
void
t3_os_link_changed(adapter_t *adapter, int port_id, int link_status, int speed,
     int duplex, int fc)
{
	struct port_info *pi = &adapter->port[port_id];
	struct cmac *mac = &adapter->port[port_id].mac;

	if (link_status) {
		DELAY(10);
		t3_mac_enable(mac, MAC_DIRECTION_RX | MAC_DIRECTION_TX);
			/* Clear errors created by MAC enable */
			t3_set_reg_field(adapter,
					 A_XGM_STAT_CTRL + pi->mac.offset,
					 F_CLRSTATS, 1);
		if_link_state_change(pi->ifp, LINK_STATE_UP);

	} else {
		pi->phy.ops->power_down(&pi->phy, 1);
		t3_mac_disable(mac, MAC_DIRECTION_RX);
		t3_link_start(&pi->phy, mac, &pi->link_config);
		t3_mac_enable(mac, MAC_DIRECTION_RX | MAC_DIRECTION_TX);
		if_link_state_change(pi->ifp, LINK_STATE_DOWN);
	}
}

/**
 *	t3_os_phymod_changed - handle PHY module changes
 *	@phy: the PHY reporting the module change
 *	@mod_type: new module type
 *
 *	This is the OS-dependent handler for PHY module changes.  It is
 *	invoked when a PHY module is removed or inserted for any OS-specific
 *	processing.
 */
void t3_os_phymod_changed(struct adapter *adap, int port_id)
{
	static const char *mod_str[] = {
		NULL, "SR", "LR", "LRM", "TWINAX", "TWINAX", "unknown"
	};

	struct port_info *pi = &adap->port[port_id];

	if (pi->phy.modtype == phy_modtype_none)
		device_printf(adap->dev, "PHY module unplugged\n");
	else {
		KASSERT(pi->phy.modtype < ARRAY_SIZE(mod_str),
		    ("invalid PHY module type %d", pi->phy.modtype));
		device_printf(adap->dev, "%s PHY module inserted\n",
		    mod_str[pi->phy.modtype]);
	}
}

/*
 * Interrupt-context handler for external (PHY) interrupts.
 */
void
t3_os_ext_intr_handler(adapter_t *sc)
{
	if (cxgb_debug)
		printf("t3_os_ext_intr_handler\n");
	/*
	 * Schedule a task to handle external interrupts as they may be slow
	 * and we use a mutex to protect MDIO registers.  We disable PHY
	 * interrupts in the meantime and let the task reenable them when
	 * it's done.
	 */
	ADAPTER_LOCK(sc);
	if (sc->slow_intr_mask) {
		sc->slow_intr_mask &= ~F_T3DBG;
		t3_write_reg(sc, A_PL_INT_ENABLE0, sc->slow_intr_mask);
		taskqueue_enqueue(sc->tq, &sc->ext_intr_task);
	}
	ADAPTER_UNLOCK(sc);
}

void
t3_os_set_hw_addr(adapter_t *adapter, int port_idx, u8 hw_addr[])
{

	/*
	 * The ifnet might not be allocated before this gets called,
	 * as this is called early on in attach by t3_prep_adapter
	 * save the address off in the port structure
	 */
	if (cxgb_debug)
		printf("set_hw_addr on idx %d addr %6D\n", port_idx, hw_addr, ":");
	bcopy(hw_addr, adapter->port[port_idx].hw_addr, ETHER_ADDR_LEN);
}

/**
 *	link_start - enable a port
 *	@p: the port to enable
 *
 *	Performs the MAC and PHY actions needed to enable a port.
 */
static void
cxgb_link_start(struct port_info *p)
{
	struct ifnet *ifp;
	struct t3_rx_mode rm;
	struct cmac *mac = &p->mac;
	int mtu, hwtagging;

	ifp = p->ifp;

	bcopy(IF_LLADDR(ifp), p->hw_addr, ETHER_ADDR_LEN);

	mtu = ifp->if_mtu;
	if (ifp->if_capenable & IFCAP_VLAN_MTU)
		mtu += ETHER_VLAN_ENCAP_LEN;

	hwtagging = (ifp->if_capenable & IFCAP_VLAN_HWTAGGING) != 0;

	t3_init_rx_mode(&rm, p);
	if (!mac->multiport) 
		t3_mac_reset(mac);
	t3_mac_set_mtu(mac, mtu);
	t3_set_vlan_accel(p->adapter, 1 << p->tx_chan, hwtagging);
	t3_mac_set_address(mac, 0, p->hw_addr);
	t3_mac_set_rx_mode(mac, &rm);
	t3_link_start(&p->phy, mac, &p->link_config);
	t3_mac_enable(mac, MAC_DIRECTION_RX | MAC_DIRECTION_TX);
}


static int
await_mgmt_replies(struct adapter *adap, unsigned long init_cnt,
			      unsigned long n)
{
	int attempts = 5;

	while (adap->sge.qs[0].rspq.offload_pkts < init_cnt + n) {
		if (!--attempts)
			return (ETIMEDOUT);
		t3_os_sleep(10);
	}
	return 0;
}

static int
init_tp_parity(struct adapter *adap)
{
	int i;
	struct mbuf *m;
	struct cpl_set_tcb_field *greq;
	unsigned long cnt = adap->sge.qs[0].rspq.offload_pkts;

	t3_tp_set_offload_mode(adap, 1);

	for (i = 0; i < 16; i++) {
		struct cpl_smt_write_req *req;

		m = m_gethdr(M_WAITOK, MT_DATA);
		req = mtod(m, struct cpl_smt_write_req *);
		m->m_len = m->m_pkthdr.len = sizeof(*req);
		memset(req, 0, sizeof(*req));
		req->wr.wr_hi = htonl(V_WR_OP(FW_WROPCODE_FORWARD));
		OPCODE_TID(req) = htonl(MK_OPCODE_TID(CPL_SMT_WRITE_REQ, i));
		req->iff = i;
		t3_mgmt_tx(adap, m);
	}

	for (i = 0; i < 2048; i++) {
		struct cpl_l2t_write_req *req;

		m = m_gethdr(M_WAITOK, MT_DATA);
		req = mtod(m, struct cpl_l2t_write_req *);
		m->m_len = m->m_pkthdr.len = sizeof(*req);
		memset(req, 0, sizeof(*req));
		req->wr.wr_hi = htonl(V_WR_OP(FW_WROPCODE_FORWARD));
		OPCODE_TID(req) = htonl(MK_OPCODE_TID(CPL_L2T_WRITE_REQ, i));
		req->params = htonl(V_L2T_W_IDX(i));
		t3_mgmt_tx(adap, m);
	}

	for (i = 0; i < 2048; i++) {
		struct cpl_rte_write_req *req;

		m = m_gethdr(M_WAITOK, MT_DATA);
		req = mtod(m, struct cpl_rte_write_req *);
		m->m_len = m->m_pkthdr.len = sizeof(*req);
		memset(req, 0, sizeof(*req));
		req->wr.wr_hi = htonl(V_WR_OP(FW_WROPCODE_FORWARD));
		OPCODE_TID(req) = htonl(MK_OPCODE_TID(CPL_RTE_WRITE_REQ, i));
		req->l2t_idx = htonl(V_L2T_W_IDX(i));
		t3_mgmt_tx(adap, m);
	}

	m = m_gethdr(M_WAITOK, MT_DATA);
	greq = mtod(m, struct cpl_set_tcb_field *);
	m->m_len = m->m_pkthdr.len = sizeof(*greq);
	memset(greq, 0, sizeof(*greq));
	greq->wr.wr_hi = htonl(V_WR_OP(FW_WROPCODE_FORWARD));
	OPCODE_TID(greq) = htonl(MK_OPCODE_TID(CPL_SET_TCB_FIELD, 0));
	greq->mask = htobe64(1);
	t3_mgmt_tx(adap, m);

	i = await_mgmt_replies(adap, cnt, 16 + 2048 + 2048 + 1);
	t3_tp_set_offload_mode(adap, 0);
	return (i);
}

/**
 *	setup_rss - configure Receive Side Steering (per-queue connection demux) 
 *	@adap: the adapter
 *
 *	Sets up RSS to distribute packets to multiple receive queues.  We
 *	configure the RSS CPU lookup table to distribute to the number of HW
 *	receive queues, and the response queue lookup table to narrow that
 *	down to the response queues actually configured for each port.
 *	We always configure the RSS mapping for two ports since the mapping
 *	table has plenty of entries.
 */
static void
setup_rss(adapter_t *adap)
{
	int i;
	u_int nq[2]; 
	uint8_t cpus[SGE_QSETS + 1];
	uint16_t rspq_map[RSS_TABLE_SIZE];
	
	for (i = 0; i < SGE_QSETS; ++i)
		cpus[i] = i;
	cpus[SGE_QSETS] = 0xff;

	nq[0] = nq[1] = 0;
	for_each_port(adap, i) {
		const struct port_info *pi = adap2pinfo(adap, i);

		nq[pi->tx_chan] += pi->nqsets;
	}
	for (i = 0; i < RSS_TABLE_SIZE / 2; ++i) {
		rspq_map[i] = nq[0] ? i % nq[0] : 0;
		rspq_map[i + RSS_TABLE_SIZE / 2] = nq[1] ? i % nq[1] + nq[0] : 0;
	}
	/* Calculate the reverse RSS map table */
	for (i = 0; i < RSS_TABLE_SIZE; ++i)
		if (adap->rrss_map[rspq_map[i]] == 0xff)
			adap->rrss_map[rspq_map[i]] = i;

	t3_config_rss(adap, F_RQFEEDBACKENABLE | F_TNLLKPEN | F_TNLMAPEN |
		      F_TNLPRTEN | F_TNL2TUPEN | F_TNL4TUPEN | F_OFDMAPEN |
	              F_RRCPLMAPEN | V_RRCPLCPUSIZE(6) | F_HASHTOEPLITZ,
	              cpus, rspq_map);

}

/*
 * Sends an mbuf to an offload queue driver
 * after dealing with any active network taps.
 */
static inline int
offload_tx(struct t3cdev *tdev, struct mbuf *m)
{
	int ret;

	ret = t3_offload_tx(tdev, m);
	return (ret);
}

static int
write_smt_entry(struct adapter *adapter, int idx)
{
	struct port_info *pi = &adapter->port[idx];
	struct cpl_smt_write_req *req;
	struct mbuf *m;

	if ((m = m_gethdr(M_NOWAIT, MT_DATA)) == NULL)
		return (ENOMEM);

	req = mtod(m, struct cpl_smt_write_req *);
	m->m_pkthdr.len = m->m_len = sizeof(struct cpl_smt_write_req);
	
	req->wr.wr_hi = htonl(V_WR_OP(FW_WROPCODE_FORWARD));
	OPCODE_TID(req) = htonl(MK_OPCODE_TID(CPL_SMT_WRITE_REQ, idx));
	req->mtu_idx = NMTUS - 1;  /* should be 0 but there's a T3 bug */
	req->iff = idx;
	memset(req->src_mac1, 0, sizeof(req->src_mac1));
	memcpy(req->src_mac0, pi->hw_addr, ETHER_ADDR_LEN);

	m_set_priority(m, 1);

	offload_tx(&adapter->tdev, m);

	return (0);
}

static int
init_smt(struct adapter *adapter)
{
	int i;

	for_each_port(adapter, i)
		write_smt_entry(adapter, i);
	return 0;
}

static void
init_port_mtus(adapter_t *adapter)
{
	unsigned int mtus = adapter->port[0].ifp->if_mtu;

	if (adapter->port[1].ifp)
		mtus |= adapter->port[1].ifp->if_mtu << 16;
	t3_write_reg(adapter, A_TP_MTU_PORT_TABLE, mtus);
}

static void
send_pktsched_cmd(struct adapter *adap, int sched, int qidx, int lo,
			      int hi, int port)
{
	struct mbuf *m;
	struct mngt_pktsched_wr *req;

	m = m_gethdr(M_DONTWAIT, MT_DATA);
	if (m) {	
		req = mtod(m, struct mngt_pktsched_wr *);
		req->wr_hi = htonl(V_WR_OP(FW_WROPCODE_MNGT));
		req->mngt_opcode = FW_MNGTOPCODE_PKTSCHED_SET;
		req->sched = sched;
		req->idx = qidx;
		req->min = lo;
		req->max = hi;
		req->binding = port;
		m->m_len = m->m_pkthdr.len = sizeof(*req);
		t3_mgmt_tx(adap, m);
	}
}

static void
bind_qsets(adapter_t *sc)
{
	int i, j;

	cxgb_pcpu_startup_threads(sc);
	for (i = 0; i < (sc)->params.nports; ++i) {
		const struct port_info *pi = adap2pinfo(sc, i);

		for (j = 0; j < pi->nqsets; ++j) {
			send_pktsched_cmd(sc, 1, pi->first_qset + j, -1,
					  -1, pi->tx_chan);

		}
	}
}

static void
update_tpeeprom(struct adapter *adap)
{
#ifdef FIRMWARE_LATEST
	const struct firmware *tpeeprom;
#else
	struct firmware *tpeeprom;
#endif

	uint32_t version;
	unsigned int major, minor;
	int ret, len;
	char rev;

	t3_seeprom_read(adap, TP_SRAM_OFFSET, &version);

	major = G_TP_VERSION_MAJOR(version);
	minor = G_TP_VERSION_MINOR(version);
	if (major == TP_VERSION_MAJOR  && minor == TP_VERSION_MINOR)
		return; 

	rev = t3rev2char(adap);

	tpeeprom = firmware_get(TPEEPROM_NAME);
	if (tpeeprom == NULL) {
		device_printf(adap->dev, "could not load TP EEPROM: unable to load %s\n",
		    TPEEPROM_NAME);
		return;
	}

	len = tpeeprom->datasize - 4;
	
	ret = t3_check_tpsram(adap, tpeeprom->data, tpeeprom->datasize);
	if (ret)
		goto release_tpeeprom;

	if (len != TP_SRAM_LEN) {
		device_printf(adap->dev, "%s length is wrong len=%d expected=%d\n", TPEEPROM_NAME, len, TP_SRAM_LEN);
		return;
	}
	
	ret = set_eeprom(&adap->port[0], tpeeprom->data, tpeeprom->datasize,
	    TP_SRAM_OFFSET);
	
	if (!ret) {
		device_printf(adap->dev,
			"Protocol SRAM image updated in EEPROM to %d.%d.%d\n",
			 TP_VERSION_MAJOR, TP_VERSION_MINOR, TP_VERSION_MICRO);
	} else 
		device_printf(adap->dev, "Protocol SRAM image update in EEPROM failed\n");

release_tpeeprom:
	firmware_put(tpeeprom, FIRMWARE_UNLOAD);
	
	return;
}

static int
update_tpsram(struct adapter *adap)
{
#ifdef FIRMWARE_LATEST
	const struct firmware *tpsram;
#else
	struct firmware *tpsram;
#endif	
	int ret;
	char rev;

	rev = t3rev2char(adap);
	if (!rev)
		return 0;

	update_tpeeprom(adap);

	tpsram = firmware_get(TPSRAM_NAME);
	if (tpsram == NULL){
		device_printf(adap->dev, "could not load TP SRAM\n");
		return (EINVAL);
	} else
		device_printf(adap->dev, "updating TP SRAM\n");
	
	ret = t3_check_tpsram(adap, tpsram->data, tpsram->datasize);
	if (ret)
		goto release_tpsram;	

	ret = t3_set_proto_sram(adap, tpsram->data);
	if (ret)
		device_printf(adap->dev, "loading protocol SRAM failed\n");

release_tpsram:
	firmware_put(tpsram, FIRMWARE_UNLOAD);
	
	return ret;
}

/**
 *	cxgb_up - enable the adapter
 *	@adap: adapter being enabled
 *
 *	Called when the first port is enabled, this function performs the
 *	actions necessary to make an adapter operational, such as completing
 *	the initialization of HW modules, and enabling interrupts.
 *
 */
static int
cxgb_up(struct adapter *sc)
{
	int err = 0;

	if ((sc->flags & FULL_INIT_DONE) == 0) {

		if ((sc->flags & FW_UPTODATE) == 0)
			if ((err = upgrade_fw(sc)))
				goto out;
		if ((sc->flags & TPS_UPTODATE) == 0)
			if ((err = update_tpsram(sc)))
				goto out;
		err = t3_init_hw(sc, 0);
		if (err)
			goto out;

		t3_set_reg_field(sc, A_TP_PARA_REG5, 0, F_RXDDPOFFINIT);
		t3_write_reg(sc, A_ULPRX_TDDP_PSZ, V_HPZ0(PAGE_SHIFT - 12));

		err = setup_sge_qsets(sc);
		if (err)
			goto out;

		setup_rss(sc);
		t3_add_configured_sysctls(sc);
		sc->flags |= FULL_INIT_DONE;
	}

	t3_intr_clear(sc);

	/* If it's MSI or INTx, allocate a single interrupt for everything */
	if ((sc->flags & USING_MSIX) == 0) {
		if ((sc->irq_res = bus_alloc_resource_any(sc->dev, SYS_RES_IRQ,
		   &sc->irq_rid, RF_SHAREABLE | RF_ACTIVE)) == NULL) {
			device_printf(sc->dev, "Cannot allocate interrupt rid=%d\n",
			    sc->irq_rid);
			err = EINVAL;
			goto out;
		}
		device_printf(sc->dev, "allocated irq_res=%p\n", sc->irq_res);

		if (bus_setup_intr(sc->dev, sc->irq_res, INTR_MPSAFE|INTR_TYPE_NET,
#ifdef INTR_FILTERS
			NULL,
#endif			
			sc->cxgb_intr, sc, &sc->intr_tag)) {
			device_printf(sc->dev, "Cannot set up interrupt\n");
			err = EINVAL;
			goto irq_err;
		}
	} else {
		cxgb_setup_msix(sc, sc->msi_count);
	}

	t3_sge_start(sc);
	t3_intr_enable(sc);

	if (sc->params.rev >= T3_REV_C && !(sc->flags & TP_PARITY_INIT) &&
	    is_offload(sc) && init_tp_parity(sc) == 0)
		sc->flags |= TP_PARITY_INIT;

	if (sc->flags & TP_PARITY_INIT) {
		t3_write_reg(sc, A_TP_INT_CAUSE,
				F_CMCACHEPERR | F_ARPLUTPERR);
		t3_write_reg(sc, A_TP_INT_ENABLE, 0x7fbfffff);
	}

	
	if (!(sc->flags & QUEUES_BOUND)) {
		bind_qsets(sc);
		sc->flags |= QUEUES_BOUND;		
	}
out:
	return (err);
irq_err:
	CH_ERR(sc, "request_irq failed, err %d\n", err);
	goto out;
}


/*
 * Release resources when all the ports and offloading have been stopped.
 */
static void
cxgb_down_locked(struct adapter *sc)
{
	
	t3_sge_stop(sc);
	t3_intr_disable(sc);
	
	if (sc->intr_tag != NULL) {
		bus_teardown_intr(sc->dev, sc->irq_res, sc->intr_tag);
		sc->intr_tag = NULL;
	}
	if (sc->irq_res != NULL) {
		device_printf(sc->dev, "de-allocating interrupt irq_rid=%d irq_res=%p\n",
		    sc->irq_rid, sc->irq_res);
		bus_release_resource(sc->dev, SYS_RES_IRQ, sc->irq_rid,
		    sc->irq_res);
		sc->irq_res = NULL;
	}
	
	if (sc->flags & USING_MSIX) 
		cxgb_teardown_msix(sc);
	
	callout_stop(&sc->cxgb_tick_ch);
	callout_stop(&sc->sge_timer_ch);
	callout_drain(&sc->cxgb_tick_ch);
	callout_drain(&sc->sge_timer_ch);
	
	if (sc->tq != NULL) {
		printf("draining slow intr\n");
		
		taskqueue_drain(sc->tq, &sc->slow_intr_task);
			printf("draining ext intr\n");	
		taskqueue_drain(sc->tq, &sc->ext_intr_task);
		printf("draining tick task\n");
		taskqueue_drain(sc->tq, &sc->tick_task);
	}
	ADAPTER_UNLOCK(sc);
}

static int
offload_open(struct port_info *pi)
{
	struct adapter *adapter = pi->adapter;
	struct t3cdev *tdev = &adapter->tdev;

	int adap_up = adapter->open_device_map & PORT_MASK;
	int err = 0;

	if (atomic_cmpset_int(&adapter->open_device_map,
		(adapter->open_device_map & ~(1<<OFFLOAD_DEVMAP_BIT)),
		(adapter->open_device_map | (1<<OFFLOAD_DEVMAP_BIT))) == 0)
		return (0);

	if (!isset(&adapter->open_device_map, OFFLOAD_DEVMAP_BIT)) 
		printf("offload_open: DEVMAP_BIT did not get set 0x%x\n",
		    adapter->open_device_map);
	ADAPTER_LOCK(pi->adapter); 
	if (!adap_up)
		err = cxgb_up(adapter);
	ADAPTER_UNLOCK(pi->adapter);
	if (err)
		return (err);

	t3_tp_set_offload_mode(adapter, 1);
	tdev->lldev = pi->ifp;

	init_port_mtus(adapter);
	t3_load_mtus(adapter, adapter->params.mtus, adapter->params.a_wnd,
		     adapter->params.b_wnd,
		     adapter->params.rev == 0 ?
		       adapter->port[0].ifp->if_mtu : 0xffff);
	init_smt(adapter);
	/* Call back all registered clients */
	cxgb_add_clients(tdev);

	/* restore them in case the offload module has changed them */
	if (err) {
		t3_tp_set_offload_mode(adapter, 0);
		clrbit(&adapter->open_device_map, OFFLOAD_DEVMAP_BIT);
		cxgb_set_dummy_ops(tdev);
	}
	return (err);
}

static int
offload_close(struct t3cdev *tdev)
{
	struct adapter *adapter = tdev2adap(tdev);

	if (!isset(&adapter->open_device_map, OFFLOAD_DEVMAP_BIT))
		return (0);

	/* Call back all registered clients */
	cxgb_remove_clients(tdev);

	tdev->lldev = NULL;
	cxgb_set_dummy_ops(tdev);
	t3_tp_set_offload_mode(adapter, 0);
	clrbit(&adapter->open_device_map, OFFLOAD_DEVMAP_BIT);

	ADAPTER_LOCK(adapter);
	if (!adapter->open_device_map)
		cxgb_down_locked(adapter);
	else
		ADAPTER_UNLOCK(adapter);
	return (0);
}


static void
cxgb_init(void *arg)
{
	struct port_info *p = arg;

	PORT_LOCK(p);
	cxgb_init_locked(p);
	PORT_UNLOCK(p);
}

static void
cxgb_init_locked(struct port_info *p)
{
	struct ifnet *ifp;
	adapter_t *sc = p->adapter;
	int err;

	PORT_LOCK_ASSERT_OWNED(p);
	ifp = p->ifp;

	ADAPTER_LOCK(p->adapter);
	if ((sc->open_device_map == 0) && (err = cxgb_up(sc))) {
		ADAPTER_UNLOCK(p->adapter);
		cxgb_stop_locked(p);
		return;
	}
	if (p->adapter->open_device_map == 0) {
		t3_intr_clear(sc);
	}
	setbit(&p->adapter->open_device_map, p->port_id);
	ADAPTER_UNLOCK(p->adapter);

	if (is_offload(sc) && !ofld_disable) {
		err = offload_open(p);
		if (err)
			log(LOG_WARNING,
			    "Could not initialize offload capabilities\n");
	}
#if !defined(LINK_ATTACH)
	cxgb_link_start(p);
	t3_link_changed(sc, p->port_id);
#endif
	ifp->if_baudrate = IF_Mbps(p->link_config.speed);

	device_printf(sc->dev, "enabling interrupts on port=%d\n", p->port_id);
	t3_port_intr_enable(sc, p->port_id);

	t3_sge_reset_adapter(sc);

	ifp->if_drv_flags |= IFF_DRV_RUNNING;
	ifp->if_drv_flags &= ~IFF_DRV_OACTIVE;
}

static void
cxgb_set_rxmode(struct port_info *p)
{
	struct t3_rx_mode rm;
	struct cmac *mac = &p->mac;

	t3_init_rx_mode(&rm, p);
	mtx_lock(&p->adapter->mdio_lock);
	t3_mac_set_rx_mode(mac, &rm);
	mtx_unlock(&p->adapter->mdio_lock);
}

static void
cxgb_stop_locked(struct port_info *pi)
{
	struct ifnet *ifp;

	PORT_LOCK_ASSERT_OWNED(pi);
	ADAPTER_LOCK_ASSERT_NOTOWNED(pi->adapter);
	
	ifp = pi->ifp;
	t3_port_intr_disable(pi->adapter, pi->port_id);
	ifp->if_drv_flags &= ~(IFF_DRV_RUNNING | IFF_DRV_OACTIVE);

	/* disable pause frames */
	t3_set_reg_field(pi->adapter, A_XGM_TX_CFG + pi->mac.offset,
			 F_TXPAUSEEN, 0);

	/* Reset RX FIFO HWM */
        t3_set_reg_field(pi->adapter, A_XGM_RXFIFO_CFG +  pi->mac.offset,
			 V_RXFIFOPAUSEHWM(M_RXFIFOPAUSEHWM), 0);


	ADAPTER_LOCK(pi->adapter);
	clrbit(&pi->adapter->open_device_map, pi->port_id);

	if (pi->adapter->open_device_map == 0) {
		cxgb_down_locked(pi->adapter);
	} else 
		ADAPTER_UNLOCK(pi->adapter);

#if !defined(LINK_ATTACH)
	DELAY(100);

	/* Wait for TXFIFO empty */
	t3_wait_op_done(pi->adapter, A_XGM_TXFIFO_CFG + pi->mac.offset,
			F_TXFIFO_EMPTY, 1, 20, 5);

	DELAY(100);
	t3_mac_disable(&pi->mac, MAC_DIRECTION_TX | MAC_DIRECTION_RX);

	pi->phy.ops->power_down(&pi->phy, 1);
#endif		

}

static int
cxgb_set_mtu(struct port_info *p, int mtu)
{
	struct ifnet *ifp = p->ifp;
	int error = 0;
	
	if ((mtu < ETHERMIN) || (mtu > ETHERMTU_JUMBO))
		error = EINVAL;
	else if (ifp->if_mtu != mtu) {
		PORT_LOCK(p);
		ifp->if_mtu = mtu;
		if (ifp->if_drv_flags & IFF_DRV_RUNNING) {
			cxgb_stop_locked(p);
			cxgb_init_locked(p);
		}
		PORT_UNLOCK(p);
	}
	return (error);
}

#ifdef LRO_SUPPORTED
/*
 * Mark lro enabled or disabled in all qsets for this port
 */
static int
cxgb_set_lro(struct port_info *p, int enabled)
{
	int i;
	struct adapter *adp = p->adapter;
	struct sge_qset *q;

	PORT_LOCK_ASSERT_OWNED(p);
	for (i = 0; i < p->nqsets; i++) {
		q = &adp->sge.qs[p->first_qset + i];
		q->lro.enabled = (enabled != 0);
	}
	return (0);
}
#endif

static int
cxgb_ioctl(struct ifnet *ifp, unsigned long command, caddr_t data)
{
	struct port_info *p = ifp->if_softc;
#ifdef INET
	struct ifaddr *ifa = (struct ifaddr *)data;
#endif
	struct ifreq *ifr = (struct ifreq *)data;
	int flags, error = 0, reinit = 0;
	uint32_t mask;

	/* 
	 * XXX need to check that we aren't in the middle of an unload
	 */
	switch (command) {
	case SIOCSIFMTU:
		error = cxgb_set_mtu(p, ifr->ifr_mtu);
		break;
	case SIOCSIFADDR:
#ifdef INET
		if (ifa->ifa_addr->sa_family == AF_INET) {
			ifp->if_flags |= IFF_UP;
			if (!(ifp->if_drv_flags & IFF_DRV_RUNNING)) {
				PORT_LOCK(p);
				cxgb_init_locked(p);
				PORT_UNLOCK(p);
			}
			arp_ifinit(ifp, ifa);
		} else
#endif
			error = ether_ioctl(ifp, command, data);
		break;
	case SIOCSIFFLAGS:
		PORT_LOCK(p);
		if (ifp->if_flags & IFF_UP) {
			if (ifp->if_drv_flags & IFF_DRV_RUNNING) {
				flags = p->if_flags;
				if (((ifp->if_flags ^ flags) & IFF_PROMISC) ||
				    ((ifp->if_flags ^ flags) & IFF_ALLMULTI))
					cxgb_set_rxmode(p);
			} else
				cxgb_init_locked(p);
			p->if_flags = ifp->if_flags;
		} else if (ifp->if_drv_flags & IFF_DRV_RUNNING)
			cxgb_stop_locked(p);
				
		PORT_UNLOCK(p);
		break;
	case SIOCADDMULTI:
	case SIOCDELMULTI:
		if (ifp->if_drv_flags & IFF_DRV_RUNNING) {
			cxgb_set_rxmode(p);
		}
		break;
	case SIOCSIFMEDIA:
	case SIOCGIFMEDIA:
		PORT_LOCK(p);
		error = ifmedia_ioctl(ifp, ifr, &p->media, command);
		PORT_UNLOCK(p);
		break;
	case SIOCSIFCAP:
		PORT_LOCK(p);
		mask = ifr->ifr_reqcap ^ ifp->if_capenable;
		if (mask & IFCAP_TXCSUM) {
			if (IFCAP_TXCSUM & ifp->if_capenable) {
				ifp->if_capenable &= ~(IFCAP_TXCSUM|IFCAP_TSO4);
				ifp->if_hwassist &= ~(CSUM_TCP | CSUM_UDP
				    | CSUM_IP | CSUM_TSO);
			} else {
				ifp->if_capenable |= IFCAP_TXCSUM;
				ifp->if_hwassist |= (CSUM_TCP | CSUM_UDP
				    | CSUM_IP);
			}
		}
		if (mask & IFCAP_RXCSUM) {
			ifp->if_capenable ^= IFCAP_RXCSUM;
		}
		if (mask & IFCAP_TSO4) {
			if (IFCAP_TSO4 & ifp->if_capenable) {
				ifp->if_capenable &= ~IFCAP_TSO4;
				ifp->if_hwassist &= ~CSUM_TSO;
			} else if (IFCAP_TXCSUM & ifp->if_capenable) {
				ifp->if_capenable |= IFCAP_TSO4;
				ifp->if_hwassist |= CSUM_TSO;
			} else {
				if (cxgb_debug)
					printf("cxgb requires tx checksum offload"
					    " be enabled to use TSO\n");
				error = EINVAL;
			}
		}
#ifdef LRO_SUPPORTED
		if (mask & IFCAP_LRO) {
			ifp->if_capenable ^= IFCAP_LRO;

			/* Safe to do this even if cxgb_up not called yet */
			cxgb_set_lro(p, ifp->if_capenable & IFCAP_LRO);
		}
#endif
		if (mask & IFCAP_VLAN_HWTAGGING) {
			ifp->if_capenable ^= IFCAP_VLAN_HWTAGGING;
			reinit = ifp->if_drv_flags & IFF_DRV_RUNNING;
		}
		if (mask & IFCAP_VLAN_MTU) {
			ifp->if_capenable ^= IFCAP_VLAN_MTU;
			reinit = ifp->if_drv_flags & IFF_DRV_RUNNING;
		}
		if (mask & IFCAP_VLAN_HWCSUM) {
			ifp->if_capenable ^= IFCAP_VLAN_HWCSUM;
		}
		if (reinit) {
			cxgb_stop_locked(p);
			cxgb_init_locked(p);
		}
		PORT_UNLOCK(p);

#ifdef VLAN_CAPABILITIES
		VLAN_CAPABILITIES(ifp);
#endif
		break;
	default:
		error = ether_ioctl(ifp, command, data);
		break;
	}
	return (error);
}

static int
cxgb_media_change(struct ifnet *ifp)
{
	if_printf(ifp, "media change not supported\n");
	return (ENXIO);
}

/*
 * Translates from phy->modtype to IFM_TYPE.
 */
static int
cxgb_ifm_type(int phymod)
{
	int rc = IFM_ETHER | IFM_FDX;

	switch (phymod) {
	case phy_modtype_sr:
		rc |= IFM_10G_SR;
		break;
	case phy_modtype_lr:
		rc |= IFM_10G_LR;
		break;
	case phy_modtype_lrm:
#ifdef IFM_10G_LRM
		rc |= IFM_10G_LRM;
#endif
		break;
	case phy_modtype_twinax:
#ifdef IFM_10G_TWINAX
		rc |= IFM_10G_TWINAX;
#endif
		break;
	case phy_modtype_twinax_long:
#ifdef IFM_10G_TWINAX_LONG
		rc |= IFM_10G_TWINAX_LONG;
#endif
		break;
	case phy_modtype_none:
		rc = IFM_ETHER | IFM_NONE;
		break;
	case phy_modtype_unknown:
		break;
	}

	return (rc);
}

static void
cxgb_media_status(struct ifnet *ifp, struct ifmediareq *ifmr)
{
	struct port_info *p = ifp->if_softc;
	struct ifmedia_entry *cur = p->media.ifm_cur;
	int m;

	if (cur->ifm_data != p->phy.modtype) { 
		/* p->media about to be rebuilt, must hold lock */
		PORT_LOCK_ASSERT_OWNED(p);

		m = cxgb_ifm_type(p->phy.modtype);
		ifmedia_removeall(&p->media);
		ifmedia_add(&p->media, m, p->phy.modtype, NULL); 
		ifmedia_set(&p->media, m);
		cur = p->media.ifm_cur; /* ifmedia_set modified ifm_cur */
		ifmr->ifm_current = m;
	}

	ifmr->ifm_status = IFM_AVALID;
	ifmr->ifm_active = IFM_ETHER;

	if (!p->link_config.link_ok)
		return;

	ifmr->ifm_status |= IFM_ACTIVE;

	switch (p->link_config.speed) {
	case 10:
		ifmr->ifm_active |= IFM_10_T;
		break;
	case 100:
		ifmr->ifm_active |= IFM_100_TX;
			break;
	case 1000:
		ifmr->ifm_active |= IFM_1000_T;
		break;
	case 10000:
		ifmr->ifm_active |= IFM_SUBTYPE(cur->ifm_media);
		break;
	}
	
	if (p->link_config.duplex)
		ifmr->ifm_active |= IFM_FDX;
	else
		ifmr->ifm_active |= IFM_HDX;
}

static void
cxgb_async_intr(void *data)
{
	adapter_t *sc = data;

	if (cxgb_debug)
		device_printf(sc->dev, "cxgb_async_intr\n");
	/*
	 * May need to sleep - defer to taskqueue
	 */
	taskqueue_enqueue(sc->tq, &sc->slow_intr_task);
}

static void
cxgb_ext_intr_handler(void *arg, int count)
{
	adapter_t *sc = (adapter_t *)arg;

	if (cxgb_debug)
		printf("cxgb_ext_intr_handler\n");

	t3_phy_intr_handler(sc);

	/* Now reenable external interrupts */
	ADAPTER_LOCK(sc);
	if (sc->slow_intr_mask) {
		sc->slow_intr_mask |= F_T3DBG;
		t3_write_reg(sc, A_PL_INT_CAUSE0, F_T3DBG);
		t3_write_reg(sc, A_PL_INT_ENABLE0, sc->slow_intr_mask);
	}
	ADAPTER_UNLOCK(sc);
}

static void
check_link_status(adapter_t *sc)
{
	int i;

	for (i = 0; i < (sc)->params.nports; ++i) {
		struct port_info *p = &sc->port[i];

		if (!(p->phy.caps & SUPPORTED_IRQ)) 
			t3_link_changed(sc, i);
		p->ifp->if_baudrate = IF_Mbps(p->link_config.speed);
	}
}

static void
check_t3b2_mac(struct adapter *adapter)
{
	int i;

	if(adapter->flags & CXGB_SHUTDOWN)
		return;
	
	for_each_port(adapter, i) {
		struct port_info *p = &adapter->port[i];
		struct ifnet *ifp = p->ifp;
		int status;
		
		if(adapter->flags & CXGB_SHUTDOWN)
			return;
		
		if ((ifp->if_drv_flags & IFF_DRV_RUNNING) == 0) 
			continue;
		
		status = 0;
		PORT_LOCK(p);
		if ((ifp->if_drv_flags & IFF_DRV_RUNNING)) 
			status = t3b2_mac_watchdog_task(&p->mac);
		if (status == 1)
			p->mac.stats.num_toggled++;
		else if (status == 2) {
			struct cmac *mac = &p->mac;
			int mtu = ifp->if_mtu;

			if (ifp->if_capenable & IFCAP_VLAN_MTU)
				mtu += ETHER_VLAN_ENCAP_LEN;
			t3_mac_set_mtu(mac, mtu);
			t3_mac_set_address(mac, 0, p->hw_addr);
			cxgb_set_rxmode(p);
			t3_link_start(&p->phy, mac, &p->link_config);
			t3_mac_enable(mac, MAC_DIRECTION_RX | MAC_DIRECTION_TX);
			t3_port_intr_enable(adapter, p->port_id);
			p->mac.stats.num_resets++;
		}
		PORT_UNLOCK(p);
	}
}

static void
cxgb_tick(void *arg)
{
	adapter_t *sc = (adapter_t *)arg;

	if(sc->flags & CXGB_SHUTDOWN)
		return;

	taskqueue_enqueue(sc->tq, &sc->tick_task);	
	callout_reset(&sc->cxgb_tick_ch, CXGB_TICKS(sc), cxgb_tick, sc);
}

static void
cxgb_tick_handler(void *arg, int count)
{
	adapter_t *sc = (adapter_t *)arg;
	const struct adapter_params *p = &sc->params;
	int i;

	if(sc->flags & CXGB_SHUTDOWN)
		return;

	ADAPTER_LOCK(sc);
	if (p->linkpoll_period)
		check_link_status(sc);

	
	sc->check_task_cnt++;

	/*
	 * adapter lock can currently only be acquired after the
	 * port lock
	 */
	ADAPTER_UNLOCK(sc);

	if (p->rev == T3_REV_B2 && p->nports < 4 && sc->open_device_map) 
		check_t3b2_mac(sc);

	for (i = 0; i < sc->params.nports; i++) {
		struct port_info *pi = &sc->port[i];
		struct ifnet *ifp = pi->ifp;
		struct mac_stats *mstats = &pi->mac.stats;
		PORT_LOCK(pi);
		t3_mac_update_stats(&pi->mac);
		PORT_UNLOCK(pi);

		
		ifp->if_opackets =
		    mstats->tx_frames_64 +
		    mstats->tx_frames_65_127 +
		    mstats->tx_frames_128_255 +
		    mstats->tx_frames_256_511 +
		    mstats->tx_frames_512_1023 +
		    mstats->tx_frames_1024_1518 +
		    mstats->tx_frames_1519_max;
		
		ifp->if_ipackets =
		    mstats->rx_frames_64 +
		    mstats->rx_frames_65_127 +
		    mstats->rx_frames_128_255 +
		    mstats->rx_frames_256_511 +
		    mstats->rx_frames_512_1023 +
		    mstats->rx_frames_1024_1518 +
		    mstats->rx_frames_1519_max;

		ifp->if_obytes = mstats->tx_octets;
		ifp->if_ibytes = mstats->rx_octets;
		ifp->if_omcasts = mstats->tx_mcast_frames;
		ifp->if_imcasts = mstats->rx_mcast_frames;
		
		ifp->if_collisions =
		    mstats->tx_total_collisions;

		ifp->if_iqdrops = mstats->rx_cong_drops;
		
		ifp->if_oerrors =
		    mstats->tx_excess_collisions +
		    mstats->tx_underrun +
		    mstats->tx_len_errs +
		    mstats->tx_mac_internal_errs +
		    mstats->tx_excess_deferral +
		    mstats->tx_fcs_errs;
		ifp->if_ierrors =
		    mstats->rx_jabber +
		    mstats->rx_data_errs +
		    mstats->rx_sequence_errs +
		    mstats->rx_runt + 
		    mstats->rx_too_long +
		    mstats->rx_mac_internal_errs +
		    mstats->rx_short +
		    mstats->rx_fcs_errs;
	}
}

static void
touch_bars(device_t dev)
{
	/*
	 * Don't enable yet
	 */
#if !defined(__LP64__) && 0
	u32 v;

	pci_read_config_dword(pdev, PCI_BASE_ADDRESS_1, &v);
	pci_write_config_dword(pdev, PCI_BASE_ADDRESS_1, v);
	pci_read_config_dword(pdev, PCI_BASE_ADDRESS_3, &v);
	pci_write_config_dword(pdev, PCI_BASE_ADDRESS_3, v);
	pci_read_config_dword(pdev, PCI_BASE_ADDRESS_5, &v);
	pci_write_config_dword(pdev, PCI_BASE_ADDRESS_5, v);
#endif
}

static int
set_eeprom(struct port_info *pi, const uint8_t *data, int len, int offset)
{
	uint8_t *buf;
	int err = 0;
	u32 aligned_offset, aligned_len, *p;
	struct adapter *adapter = pi->adapter;


	aligned_offset = offset & ~3;
	aligned_len = (len + (offset & 3) + 3) & ~3;

	if (aligned_offset != offset || aligned_len != len) {
		buf = malloc(aligned_len, M_DEVBUF, M_WAITOK|M_ZERO);		   
		if (!buf)
			return (ENOMEM);
		err = t3_seeprom_read(adapter, aligned_offset, (u32 *)buf);
		if (!err && aligned_len > 4)
			err = t3_seeprom_read(adapter,
					      aligned_offset + aligned_len - 4,
					      (u32 *)&buf[aligned_len - 4]);
		if (err)
			goto out;
		memcpy(buf + (offset & 3), data, len);
	} else
		buf = (uint8_t *)(uintptr_t)data;

	err = t3_seeprom_wp(adapter, 0);
	if (err)
		goto out;

	for (p = (u32 *)buf; !err && aligned_len; aligned_len -= 4, p++) {
		err = t3_seeprom_write(adapter, aligned_offset, *p);
		aligned_offset += 4;
	}

	if (!err)
		err = t3_seeprom_wp(adapter, 1);
out:
	if (buf != data)
		free(buf, M_DEVBUF);
	return err;
}


static int
in_range(int val, int lo, int hi)
{
	return val < 0 || (val <= hi && val >= lo);
}

static int
cxgb_extension_open(struct cdev *dev, int flags, int fmp, d_thread_t *td)
{
       return (0);
}

static int
cxgb_extension_close(struct cdev *dev, int flags, int fmt, d_thread_t *td)
{
       return (0);
}

static int
cxgb_extension_ioctl(struct cdev *dev, unsigned long cmd, caddr_t data,
    int fflag, struct thread *td)
{
	int mmd, error = 0;
	struct port_info *pi = dev->si_drv1;
	adapter_t *sc = pi->adapter;

#ifdef PRIV_SUPPORTED	
	if (priv_check(td, PRIV_DRIVER)) {
		if (cxgb_debug) 
			printf("user does not have access to privileged ioctls\n");
		return (EPERM);
	}
#else
	if (suser(td)) {
		if (cxgb_debug)
			printf("user does not have access to privileged ioctls\n");
		return (EPERM);
	}
#endif
	
	switch (cmd) {
	case CHELSIO_GET_MIIREG: {
		uint32_t val;
		struct cphy *phy = &pi->phy;
		struct ch_mii_data *mid = (struct ch_mii_data *)data;
		
		if (!phy->mdio_read)
			return (EOPNOTSUPP);
		if (is_10G(sc)) {
			mmd = mid->phy_id >> 8;
			if (!mmd)
				mmd = MDIO_DEV_PCS;
			else if (mmd > MDIO_DEV_XGXS)
				return (EINVAL);

			error = phy->mdio_read(sc, mid->phy_id & 0x1f, mmd,
					     mid->reg_num, &val);
		} else
		        error = phy->mdio_read(sc, mid->phy_id & 0x1f, 0,
					     mid->reg_num & 0x1f, &val);
		if (error == 0)
			mid->val_out = val;
		break;
	}
	case CHELSIO_SET_MIIREG: {
		struct cphy *phy = &pi->phy;
		struct ch_mii_data *mid = (struct ch_mii_data *)data;

		if (!phy->mdio_write)
			return (EOPNOTSUPP);
		if (is_10G(sc)) {
			mmd = mid->phy_id >> 8;
			if (!mmd)
				mmd = MDIO_DEV_PCS;
			else if (mmd > MDIO_DEV_XGXS)
				return (EINVAL);
			
			error = phy->mdio_write(sc, mid->phy_id & 0x1f,
					      mmd, mid->reg_num, mid->val_in);
		} else
			error = phy->mdio_write(sc, mid->phy_id & 0x1f, 0,
					      mid->reg_num & 0x1f,
					      mid->val_in);
		break;
	}
	case CHELSIO_SETREG: {
		struct ch_reg *edata = (struct ch_reg *)data;
		if ((edata->addr & 0x3) != 0 || edata->addr >= sc->mmio_len)
			return (EFAULT);
		t3_write_reg(sc, edata->addr, edata->val);
		break;
	}
	case CHELSIO_GETREG: {
		struct ch_reg *edata = (struct ch_reg *)data;
		if ((edata->addr & 0x3) != 0 || edata->addr >= sc->mmio_len)
			return (EFAULT);
		edata->val = t3_read_reg(sc, edata->addr);
		break;
	}
	case CHELSIO_GET_SGE_CONTEXT: {
		struct ch_cntxt *ecntxt = (struct ch_cntxt *)data;
		mtx_lock_spin(&sc->sge.reg_lock);
		switch (ecntxt->cntxt_type) {
		case CNTXT_TYPE_EGRESS:
			error = -t3_sge_read_ecntxt(sc, ecntxt->cntxt_id,
			    ecntxt->data);
			break;
		case CNTXT_TYPE_FL:
			error = -t3_sge_read_fl(sc, ecntxt->cntxt_id,
			    ecntxt->data);
			break;
		case CNTXT_TYPE_RSP:
			error = -t3_sge_read_rspq(sc, ecntxt->cntxt_id,
			    ecntxt->data);
			break;
		case CNTXT_TYPE_CQ:
			error = -t3_sge_read_cq(sc, ecntxt->cntxt_id,
			    ecntxt->data);
			break;
		default:
			error = EINVAL;
			break;
		}
		mtx_unlock_spin(&sc->sge.reg_lock);
		break;
	}
	case CHELSIO_GET_SGE_DESC: {
		struct ch_desc *edesc = (struct ch_desc *)data;
		int ret;
		if (edesc->queue_num >= SGE_QSETS * 6)
			return (EINVAL);
		ret = t3_get_desc(&sc->sge.qs[edesc->queue_num / 6],
		    edesc->queue_num % 6, edesc->idx, edesc->data);
		if (ret < 0)
			return (EINVAL);
		edesc->size = ret;
		break;
	}
	case CHELSIO_GET_QSET_PARAMS: {
		struct qset_params *q;
		struct ch_qset_params *t = (struct ch_qset_params *)data;
		int q1 = pi->first_qset;
		int nqsets = pi->nqsets;
		int i;

		if (t->qset_idx >= nqsets)
			return EINVAL;

		i = q1 + t->qset_idx;
		q = &sc->params.sge.qset[i];
		t->rspq_size   = q->rspq_size;
		t->txq_size[0] = q->txq_size[0];
		t->txq_size[1] = q->txq_size[1];
		t->txq_size[2] = q->txq_size[2];
		t->fl_size[0]  = q->fl_size;
		t->fl_size[1]  = q->jumbo_size;
		t->polling     = q->polling;
		t->lro         = q->lro;
		t->intr_lat    = q->coalesce_usecs;
		t->cong_thres  = q->cong_thres;
		t->qnum        = i;

		if (sc->flags & USING_MSIX)
			t->vector = rman_get_start(sc->msix_irq_res[i]);
		else
			t->vector = rman_get_start(sc->irq_res);

		break;
	}
	case CHELSIO_GET_QSET_NUM: {
		struct ch_reg *edata = (struct ch_reg *)data;
		edata->val = pi->nqsets;
		break;
	}
	case CHELSIO_LOAD_FW: {
		uint8_t *fw_data;
		uint32_t vers;
		struct ch_mem_range *t = (struct ch_mem_range *)data;

		/*
		 * You're allowed to load a firmware only before FULL_INIT_DONE
		 *
		 * FW_UPTODATE is also set so the rest of the initialization
		 * will not overwrite what was loaded here.  This gives you the
		 * flexibility to load any firmware (and maybe shoot yourself in
		 * the foot).
		 */

		ADAPTER_LOCK(sc);
		if (sc->open_device_map || sc->flags & FULL_INIT_DONE) {
			ADAPTER_UNLOCK(sc);
			return (EBUSY);
		}

		fw_data = malloc(t->len, M_DEVBUF, M_NOWAIT);
		if (!fw_data)
			error = ENOMEM;
		else
			error = copyin(t->buf, fw_data, t->len);

		if (!error)
			error = -t3_load_fw(sc, fw_data, t->len);

		if (t3_get_fw_version(sc, &vers) == 0) {
			snprintf(&sc->fw_version[0], sizeof(sc->fw_version),
			    "%d.%d.%d", G_FW_VERSION_MAJOR(vers),
			    G_FW_VERSION_MINOR(vers), G_FW_VERSION_MICRO(vers));
		}

		if (!error)
			sc->flags |= FW_UPTODATE;

		free(fw_data, M_DEVBUF);
		ADAPTER_UNLOCK(sc);
		break;
	}
	case CHELSIO_LOAD_BOOT: {
		uint8_t *boot_data;
		struct ch_mem_range *t = (struct ch_mem_range *)data;

		boot_data = malloc(t->len, M_DEVBUF, M_NOWAIT);
		if (!boot_data)
			return ENOMEM;

		error = copyin(t->buf, boot_data, t->len);
		if (!error)
			error = -t3_load_boot(sc, boot_data, t->len);

		free(boot_data, M_DEVBUF);
		break;
	}
	case CHELSIO_GET_PM: {
		struct ch_pm *m = (struct ch_pm *)data;
		struct tp_params *p = &sc->params.tp;

		if (!is_offload(sc))
			return (EOPNOTSUPP);

		m->tx_pg_sz = p->tx_pg_size;
		m->tx_num_pg = p->tx_num_pgs;
		m->rx_pg_sz  = p->rx_pg_size;
		m->rx_num_pg = p->rx_num_pgs;
		m->pm_total  = p->pmtx_size + p->chan_rx_size * p->nchan;

		break;
	}
	case CHELSIO_SET_PM: {
		struct ch_pm *m = (struct ch_pm *)data;
		struct tp_params *p = &sc->params.tp;

		if (!is_offload(sc))
			return (EOPNOTSUPP);
		if (sc->flags & FULL_INIT_DONE)
			return (EBUSY);

		if (!m->rx_pg_sz || (m->rx_pg_sz & (m->rx_pg_sz - 1)) ||
		    !m->tx_pg_sz || (m->tx_pg_sz & (m->tx_pg_sz - 1)))
			return (EINVAL);	/* not power of 2 */
		if (!(m->rx_pg_sz & 0x14000))
			return (EINVAL);	/* not 16KB or 64KB */
		if (!(m->tx_pg_sz & 0x1554000))
			return (EINVAL);
		if (m->tx_num_pg == -1)
			m->tx_num_pg = p->tx_num_pgs;
		if (m->rx_num_pg == -1)
			m->rx_num_pg = p->rx_num_pgs;
		if (m->tx_num_pg % 24 || m->rx_num_pg % 24)
			return (EINVAL);
		if (m->rx_num_pg * m->rx_pg_sz > p->chan_rx_size ||
		    m->tx_num_pg * m->tx_pg_sz > p->chan_tx_size)
			return (EINVAL);

		p->rx_pg_size = m->rx_pg_sz;
		p->tx_pg_size = m->tx_pg_sz;
		p->rx_num_pgs = m->rx_num_pg;
		p->tx_num_pgs = m->tx_num_pg;
		break;
	}
	case CHELSIO_SETMTUTAB: {
		struct ch_mtus *m = (struct ch_mtus *)data;
		int i;
		
		if (!is_offload(sc))
			return (EOPNOTSUPP);
		if (offload_running(sc))
			return (EBUSY);
		if (m->nmtus != NMTUS)
			return (EINVAL);
		if (m->mtus[0] < 81)         /* accommodate SACK */
			return (EINVAL);
		
		/*
		 * MTUs must be in ascending order
		 */
		for (i = 1; i < NMTUS; ++i)
			if (m->mtus[i] < m->mtus[i - 1])
				return (EINVAL);

		memcpy(sc->params.mtus, m->mtus, sizeof(sc->params.mtus));
		break;
	}
	case CHELSIO_GETMTUTAB: {
		struct ch_mtus *m = (struct ch_mtus *)data;

		if (!is_offload(sc))
			return (EOPNOTSUPP);

		memcpy(m->mtus, sc->params.mtus, sizeof(m->mtus));
		m->nmtus = NMTUS;
		break;
	}
	case CHELSIO_GET_MEM: {
		struct ch_mem_range *t = (struct ch_mem_range *)data;
		struct mc7 *mem;
		uint8_t *useraddr;
		u64 buf[32];

		/*
		 * Use these to avoid modifying len/addr in the the return
		 * struct
		 */
		uint32_t len = t->len, addr = t->addr;

		if (!is_offload(sc))
			return (EOPNOTSUPP);
		if (!(sc->flags & FULL_INIT_DONE))
			return (EIO);         /* need the memory controllers */
		if ((addr & 0x7) || (len & 0x7))
			return (EINVAL);
		if (t->mem_id == MEM_CM)
			mem = &sc->cm;
		else if (t->mem_id == MEM_PMRX)
			mem = &sc->pmrx;
		else if (t->mem_id == MEM_PMTX)
			mem = &sc->pmtx;
		else
			return (EINVAL);

		/*
		 * Version scheme:
		 * bits 0..9: chip version
		 * bits 10..15: chip revision
		 */
		t->version = 3 | (sc->params.rev << 10);
		
		/*
		 * Read 256 bytes at a time as len can be large and we don't
		 * want to use huge intermediate buffers.
		 */
		useraddr = (uint8_t *)t->buf; 
		while (len) {
			unsigned int chunk = min(len, sizeof(buf));

			error = t3_mc7_bd_read(mem, addr / 8, chunk / 8, buf);
			if (error)
				return (-error);
			if (copyout(buf, useraddr, chunk))
				return (EFAULT);
			useraddr += chunk;
			addr += chunk;
			len -= chunk;
		}
		break;
	}
	case CHELSIO_READ_TCAM_WORD: {
		struct ch_tcam_word *t = (struct ch_tcam_word *)data;

		if (!is_offload(sc))
			return (EOPNOTSUPP);
		if (!(sc->flags & FULL_INIT_DONE))
			return (EIO);         /* need MC5 */		
		return -t3_read_mc5_range(&sc->mc5, t->addr, 1, t->buf);
		break;
	}
	case CHELSIO_SET_TRACE_FILTER: {
		struct ch_trace *t = (struct ch_trace *)data;
		const struct trace_params *tp;

		tp = (const struct trace_params *)&t->sip;
		if (t->config_tx)
			t3_config_trace_filter(sc, tp, 0, t->invert_match,
					       t->trace_tx);
		if (t->config_rx)
			t3_config_trace_filter(sc, tp, 1, t->invert_match,
					       t->trace_rx);
		break;
	}
	case CHELSIO_SET_PKTSCHED: {
		struct ch_pktsched_params *p = (struct ch_pktsched_params *)data;
		if (sc->open_device_map == 0)
			return (EAGAIN);
		send_pktsched_cmd(sc, p->sched, p->idx, p->min, p->max,
		    p->binding);
		break;
	}
	case CHELSIO_IFCONF_GETREGS: {
		struct ch_ifconf_regs *regs = (struct ch_ifconf_regs *)data;
		int reglen = cxgb_get_regs_len();
		uint8_t *buf = malloc(reglen, M_DEVBUF, M_NOWAIT);
		if (buf == NULL) {
			return (ENOMEM);
		}
		if (regs->len > reglen)
			regs->len = reglen;
		else if (regs->len < reglen)
			error = E2BIG;

		if (!error) {
			cxgb_get_regs(sc, regs, buf);
			error = copyout(buf, regs->data, reglen);
		}
		free(buf, M_DEVBUF);

		break;
	}
	case CHELSIO_SET_HW_SCHED: {
		struct ch_hw_sched *t = (struct ch_hw_sched *)data;
		unsigned int ticks_per_usec = core_ticks_per_usec(sc);

		if ((sc->flags & FULL_INIT_DONE) == 0)
			return (EAGAIN);       /* need TP to be initialized */
		if (t->sched >= NTX_SCHED || !in_range(t->mode, 0, 1) ||
		    !in_range(t->channel, 0, 1) ||
		    !in_range(t->kbps, 0, 10000000) ||
		    !in_range(t->class_ipg, 0, 10000 * 65535 / ticks_per_usec) ||
		    !in_range(t->flow_ipg, 0,
			      dack_ticks_to_usec(sc, 0x7ff)))
			return (EINVAL);

		if (t->kbps >= 0) {
			error = t3_config_sched(sc, t->kbps, t->sched);
			if (error < 0)
				return (-error);
		}
		if (t->class_ipg >= 0)
			t3_set_sched_ipg(sc, t->sched, t->class_ipg);
		if (t->flow_ipg >= 0) {
			t->flow_ipg *= 1000;     /* us -> ns */
			t3_set_pace_tbl(sc, &t->flow_ipg, t->sched, 1);
		}
		if (t->mode >= 0) {
			int bit = 1 << (S_TX_MOD_TIMER_MODE + t->sched);

			t3_set_reg_field(sc, A_TP_TX_MOD_QUEUE_REQ_MAP,
					 bit, t->mode ? bit : 0);
		}
		if (t->channel >= 0)
			t3_set_reg_field(sc, A_TP_TX_MOD_QUEUE_REQ_MAP,
					 1 << t->sched, t->channel << t->sched);
		break;
	}
	case CHELSIO_GET_EEPROM: {
		int i;
		struct ch_eeprom *e = (struct ch_eeprom *)data;
		uint8_t *buf = malloc(EEPROMSIZE, M_DEVBUF, M_NOWAIT);

		if (buf == NULL) {
			return (ENOMEM);
		}
		e->magic = EEPROM_MAGIC;
		for (i = e->offset & ~3; !error && i < e->offset + e->len; i += 4)
			error = -t3_seeprom_read(sc, i, (uint32_t *)&buf[i]);

		if (!error)
			error = copyout(buf + e->offset, e->data, e->len);

		free(buf, M_DEVBUF);
		break;
	}
	case CHELSIO_CLEAR_STATS: {
		if (!(sc->flags & FULL_INIT_DONE))
			return EAGAIN;

		PORT_LOCK(pi);
		t3_mac_update_stats(&pi->mac);
		memset(&pi->mac.stats, 0, sizeof(pi->mac.stats));
		PORT_UNLOCK(pi);
		break;
	}
	default:
		return (EOPNOTSUPP);
		break;
	}

	return (error);
}

static __inline void
reg_block_dump(struct adapter *ap, uint8_t *buf, unsigned int start,
    unsigned int end)
{
	uint32_t *p = (uint32_t *)(buf + start);

	for ( ; start <= end; start += sizeof(uint32_t))
		*p++ = t3_read_reg(ap, start);
}

#define T3_REGMAP_SIZE (3 * 1024)
static int
cxgb_get_regs_len(void)
{
	return T3_REGMAP_SIZE;
}

static void
cxgb_get_regs(adapter_t *sc, struct ch_ifconf_regs *regs, uint8_t *buf)
{	    
	
	/*
	 * Version scheme:
	 * bits 0..9: chip version
	 * bits 10..15: chip revision
	 * bit 31: set for PCIe cards
	 */
	regs->version = 3 | (sc->params.rev << 10) | (is_pcie(sc) << 31);

	/*
	 * We skip the MAC statistics registers because they are clear-on-read.
	 * Also reading multi-register stats would need to synchronize with the
	 * periodic mac stats accumulation.  Hard to justify the complexity.
	 */
	memset(buf, 0, cxgb_get_regs_len());
	reg_block_dump(sc, buf, 0, A_SG_RSPQ_CREDIT_RETURN);
	reg_block_dump(sc, buf, A_SG_HI_DRB_HI_THRSH, A_ULPRX_PBL_ULIMIT);
	reg_block_dump(sc, buf, A_ULPTX_CONFIG, A_MPS_INT_CAUSE);
	reg_block_dump(sc, buf, A_CPL_SWITCH_CNTRL, A_CPL_MAP_TBL_DATA);
	reg_block_dump(sc, buf, A_SMB_GLOBAL_TIME_CFG, A_XGM_SERDES_STAT3);
	reg_block_dump(sc, buf, A_XGM_SERDES_STATUS0,
		       XGM_REG(A_XGM_SERDES_STAT3, 1));
	reg_block_dump(sc, buf, XGM_REG(A_XGM_SERDES_STATUS0, 1),
		       XGM_REG(A_XGM_RX_SPI4_SOP_EOP_CNT, 1));
}


MODULE_DEPEND(if_cxgb, cxgb_t3fw, 1, 1, 1);
