// Include section
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <linux/delay.h>

// Define section
#define PERIPH_BASE 0x20000000
#define GPIO_OFFSET 0x00200000

#define DEVICE_NAME "LCD_16x2"

// Function prototype section
static void GPIO_SetFunction(int GPIO, int function);
static void GPIO_SetValue(int GPIO, bool value);

int LCD_16x2_open(struct inode *pinode, struct file *pfile);
ssize_t LCD_16x2_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset);
int LCD_16x2_close(struct inode *pinode, struct file *pfile);

// Structs section
struct GPIO_Registers
{
	uint32_t GPFSEL[6];
	uint32_t reserved0;
	uint32_t GPSET[2];
	uint32_t reserved1;
	uint32_t GPCLR[2];
};

// Variables section
int rs_pin = 17;
int en_pin = 27;
int d_pin[8] = { 22, 23, 24, 10, 9, 25, 11, 8 };

int major_number = 0;

struct GPIO_Registers* gpio_registers;

struct file_operations LCD_16x2_file_operations =
{
	.owner = THIS_MODULE,
	.open = LCD_16x2_open,
	.write = LCD_16x2_write,
	.release = LCD_16x2_close,
};

static void GPIO_SetFunction(int GPIO, int function)
{
	int register_index = GPIO / 10;
	int bit = (GPIO % 10) * 3;
	uint32_t mask = 0b111 << bit;
	
	uint32_t old_value = gpio_registers->GPFSEL[register_index];
	
	gpio_registers->GPFSEL[register_index] = (old_value & ~mask) | ((function << bit) & mask);
}

static void GPIO_SetValue(int GPIO, bool value)
{
	if (value)
		gpio_registers->GPSET[GPIO / 32] = (1 << (GPIO % 32));
	else
		gpio_registers->GPCLR[GPIO / 32] = (1 << (GPIO % 32));
}

int LCD_16x2_open(struct inode *pinode, struct file *pfile)
{
	
	return 0;
}

ssize_t LCD_16x2_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
	if (length > 1)
	{
		int i, j;
		switch(buffer[0])
		{
			// Write to LCD's command register
			case 0:
			{
				GPIO_SetValue(rs_pin, 0);
				GPIO_SetValue(en_pin, 1);
				for (i = 1; i < length; i++)
				{
					for (j = 0; j < 8; j++)
					{
						GPIO_SetValue(d_pin[j], ((buffer[i] >> j) & 1));
					}
					GPIO_SetValue(en_pin, 0);
					GPIO_SetValue(en_pin, 1);
					mdelay(1);
				}
				break;
			}
			// Write to LCD's data register
			case 1:
			{
				GPIO_SetValue(rs_pin, 1);
				GPIO_SetValue(en_pin, 1);
				for (i = 1; i < length; i++)
				{
					for (j = 0; j < 8; j++)
					{
						GPIO_SetValue(d_pin[j], ((buffer[i] >> j) & 1));
					}
					GPIO_SetValue(en_pin, 0);
					GPIO_SetValue(en_pin, 1);
					mdelay(1);
				}
				break;
			}
			default: break;
		}
	}
	return length;
}

int LCD_16x2_close(struct inode *pinode, struct file *pfile)
{
	return 0;
}

static int LCD_16x2_init(void)
{
	major_number = register_chrdev(0, DEVICE_NAME, &LCD_16x2_file_operations);	// register character device and get dynamic major number
	gpio_registers = (struct GPIO_Registers*)ioremap(PERIPH_BASE + GPIO_OFFSET, sizeof(struct GPIO_Registers));
	GPIO_SetFunction(rs_pin, 0b001);
	GPIO_SetFunction(en_pin, 0b001);
	GPIO_SetFunction(d_pin[0], 0b001);
	GPIO_SetFunction(d_pin[1], 0b001);
	GPIO_SetFunction(d_pin[2], 0b001);
	GPIO_SetFunction(d_pin[3], 0b001);
	GPIO_SetFunction(d_pin[4], 0b001);
	GPIO_SetFunction(d_pin[5], 0b001);
	GPIO_SetFunction(d_pin[6], 0b001);
	GPIO_SetFunction(d_pin[7], 0b001);
	GPIO_SetValue(rs_pin, 1);
	GPIO_SetValue(en_pin, 1);
	GPIO_SetValue(d_pin[0], 1);
	GPIO_SetValue(d_pin[1], 1);
	GPIO_SetValue(d_pin[2], 1);
	GPIO_SetValue(d_pin[3], 1);
	GPIO_SetValue(d_pin[4], 1);
	GPIO_SetValue(d_pin[5], 1);
	GPIO_SetValue(d_pin[6], 1);
	GPIO_SetValue(d_pin[7], 1);
	return 0;
}

static void LCD_16x2_exit(void)
{
	unregister_chrdev(major_number, DEVICE_NAME);	// unregister character device
	iounmap(gpio_registers);
	printk(KERN_ALERT "st7735 driver module unloaded\n");
}

module_init(LCD_16x2_init);
module_exit(LCD_16x2_exit);

module_param(rs_pin, int, 0);
module_param(en_pin, int, 0);
module_param_array(d_pin, int, NULL, 0);

