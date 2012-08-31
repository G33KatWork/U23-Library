#include <USB_Device.h>
#include <USB_DeviceDescriptor.h>
#include <usbd/usbd_req.h>

static uint8_t USBD_DEV_Init(void *pdev, uint8_t cfgidx);
static uint8_t USBD_DEV_DeInit(void *pdev, uint8_t cfgidx);
static uint8_t USBD_DEV_Setup(void *pdev, USB_SETUP_REQ *req);
static uint8_t *USBD_DEV_GetCfgDesc(uint8_t speed, uint16_t *length);

USBD_Class_cb_TypeDef USBD_DEV_cb = 
{
  USBD_DEV_Init,
  USBD_DEV_DeInit,
  USBD_DEV_Setup,
  NULL, /*EP0_TxSent*/  
  NULL, /*EP0_RxReady*/
  NULL, /*DataIn*/
  NULL, /*DataOut*/
  NULL, /*SOF */
  NULL,
  NULL,      
  USBD_DEV_GetCfgDesc
};

__ALIGN_BEGIN static uint8_t USBD_DEV_CfgDesc[USB_DEV_CONFIG_DESC_SIZ] __ALIGN_END =
{
  0x09, /* bLength: Configuration Descriptor size */
  USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType: Configuration */
  USB_DEV_CONFIG_DESC_SIZ, 0x00, /* wTotalLength: Bytes returned */
  0x01,         /*bNumInterfaces: 1 interface*/
  0x01,         /*bConfigurationValue: Configuration value*/
  USBD_IDX_CONFIG_STR,         /*iConfiguration: Index of string descriptor describing the configuration*/
  0xC0,         /*bmAttributes: bus powered */
  0x32,         /*MaxPower 100 mA: this current is used for detecting Vbus*/
  
  /************** Interface descriptor ****************/
  /* 09 */
  0x09,         /*bLength: Interface Descriptor size*/
  USB_INTERFACE_DESCRIPTOR_TYPE,/*bDescriptorType: Interface descriptor type*/
  0x00,         /*bInterfaceNumber: Number of Interface*/
  0x00,         /*bAlternateSetting: Alternate setting*/
  0x00,         /*bNumEndpoints*/
  0xFF,         /*bInterfaceClass*/
  0x00,         /*bInterfaceSubClass*/
  0x00,         /*nInterfaceProtocol*/
  USBD_IDX_INTERFACE_STR,         /*iInterface: Index of string descriptor*/
  /* 18 */
};

static uint8_t USBD_DEV_Init (void  *pdev, uint8_t cfgidx)
{
  return USBD_OK;
}

static uint8_t USBD_DEV_DeInit(void *pdev, uint8_t cfgidx)
{
  return USBD_OK;
}

static uint8_t USBD_DEV_Setup(void *pdev, USB_SETUP_REQ *req)
{
  return USBD_OK;
}

static uint8_t *USBD_DEV_GetCfgDesc(uint8_t speed, uint16_t *length)
{
  *length = sizeof(USBD_DEV_CfgDesc);
  return USBD_DEV_CfgDesc;
}
