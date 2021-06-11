# usb camera demo code
# uvc drv linux kernel menuconfig
	->device drivers
		-->usb support
		-->usb gadget support
			--->usb webcam function (*)
			--->usb webcam gadget (*)
	->device drivers
		-->multimedia support
			--->media usb adapters
				---->usb video class (uvc) (*)
						----->uvc input events device support (*)
 # Compile to generate driver KO
 1.   linux-5.4/drivers/media/usb/uvc/uvcvideo.ko  
 2.   linux-5.4/drivers/media/common/videobuf2/videobuf2-vmalloc.ko  
 3.   linux-5.4/drivers/media/common/videobuf2/videobuf2-memops.ko  
# insmod ko
1. insmod videobuf2-memops.ko
2. insmod videobuf2-vmalloc.ko
3. insmod uvcvideo.ko
