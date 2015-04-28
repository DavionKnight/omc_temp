/*******************************************************************************
********************************************************************************
* 文件名称:  omap_pinmux.h
* 功能描述:  对omap各个管脚功能初始化的定义。主要分三个定义。
*						一.XXX_XXX_PINMUXREG 应用XXX_XXX功能的管脚对应的pinmux寄存器数（0-19）
*						二.XXX_XXX_PINMUX 该管脚在pingmux寄存器中的位置。32位寄存器（0-7）
*						三.PINMUX_CONFIG_XXX 该管脚被配置成XXX功能。
* 使用说明: 将需要配置管脚的三项数据定义在该文件中。配置PINMUX 用MUX 宏定义
* 文件作者:	H4
* 编写日期: （2012/06/18）
* 修改历史:
* 修改日期    修改人       修改内容
*-------------------------------------------------------------------------------

*******************************************************************************/
#ifndef _OMAP_PINMUX_H
#define _OMAP_PINMUX_H

#define PINMUX_PHY_ADDR_BASE 0x01C14000
#define PINMUX_ADDR_OFFSET 0x120

/*选择在哪一个pinmux寄存器里配置*/
#define TMP_SCL_REG 17
#define TMP_SDA_REG 17
#define KSZ8873_MDC_PINMUXREG 4
#define KSZ8873_MDIO_PINMUXREG  4
#define SPI_CLK_PINMUXREG 5
#define SPI_MISO_PINMUXREG 5
#define SPI_MOSI_PINMUXREG 5
#define SPI_LE_PINMUXREG 5
#define SPI_CS0_PINMUXREG 5
#define SPI_CS1_PINMUXREG 5
#define OMAP_I2C_SCL_PINMUXREG 4
#define OMAP_I2C_SDA_PINMUXREG 4
#define RS485_RX_PINMUXREG 4
#define RS485_TX_PINMUXREG 4
#define RS485_LE_PINMUXREG 0
#define EPLD_INT1_PINMUXREG 0
#define EPLD_INT2_PINMUXREG 0
#define KSZ8873_INT_PINMUXREG 1
#define FPGA_INT_PINMUXREG 0
#define EMIFA_CLK_PINMUXREG 6


/*pinmux中寄存器的第几个配置项*/
#define TMP_SCL_PIN  3
#define TMP_SDA_PIN  4
#define KSZ8873_MDC_PINMUX 0
#define KSZ8873_MDIO_PINMUX 1
#define SPI_CLK_PINMUX 2
#define SPI_MISO_PINMUX 4
#define SPI_MOSI_PINMUX 5
#define SPI_LE_PINMUX 3
#define SPI_CS0_PINMUX 1
#define SPI_CS1_PINMUX 0
#define OMAP_I2C_SCL_PINMUX 2
#define OMAP_I2C_SDA_PINMUX 3
#define RS485_RX_PINMUX 4
#define RS485_TX_PINMUX 5
#define RS485_LE_PINMUX 7
#define EPLD_INT1_PINMUX 3
#define EPLD_INT2_PINMUX 6
#define KSZ8873_INT_PINMUX 0
#define FPGA_INT_PINMUX 5
#define EMIFA_CLK_PINMUX 0
  

/*pinmux中配置的值*/
#define PINMUX_CONFIG_GPIO  0x00000008
#define PINMUX_CONFIG_MD_GPIO  0x00000004
#define PINMUX_CONFIG_UART  0x00000002
#define PINMUX_CONFIG_EMIFA 0x00000001


typedef struct {
   volatile unsigned int pinmux[20];
}pinmux_reg;


#define MUX(regindex,leftindex,value) pinmux_t->pinmux[regindex]=(((pinmux_t->pinmux[regindex])&(~(0x0000000f<<(leftindex*4))))|(value<<(leftindex*4)))


extern pinmux_reg *pinmux_t;

#endif 
