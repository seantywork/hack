
/* The 'magic' number for our driver; see
 * Documentation/ioctl/ioctl-number.rst
 * Of course, we don't know for _sure_ if the magic # we choose here this
 * will remain free; it really doesn't matter, this is just for demo purposes;
 * don't try and upstream this without further investigation :-)
 */
#define IOCTL_LLKD_MAGIC		0xA8

#define	IOCTL_LLKD_MAXIOCTL		3
/*
 * The _IO{R|W}() macros can be summarized as follows:
_IO(type,nr)                  ioctl command with no argument
_IOR(type,nr,datatype)        ioctl command for reading data from the kernel/drv
_IOW(type,nr,datatype)        ioctl command for writing data to the kernel/drv
_IOWR(type,nr,datatype)       ioctl command for read/write transfers
*/
/* our dummy ioctl (IOC) RESET command */
#define IOCTL_LLKD_IOCRESET		_IO(IOCTL_LLKD_MAGIC, 0)

/* our dummy ioctl (IOC) Query POWER command */
#define IOCTL_LLKD_IOCQPOWER		_IOR(IOCTL_LLKD_MAGIC, 1, int)

/* our dummy ioctl (IOC) Set POWER command */
#define IOCTL_LLKD_IOCSPOWER		_IOW(IOCTL_LLKD_MAGIC, 2, int)