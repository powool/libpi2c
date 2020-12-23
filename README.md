PI I/O chip: BCM 2835
i2ckernelmemory is implemented using a kernelmemory
i2cbus is implemented using a i2ckernelmemory
i2cbusextender is implemented using an i2cbus
i2cbus and i2cbusextender are identical in external form

i2ckernelmemory uses a single spin lock to protect access to i2c I/O?

KernelMemoryCreate()
I2cKernelMemoryCreate(KernelMemory)
I2cBusCreate(I2cKernelMemory)
I2cBusExtenderCreate(I2cBus)

template<>
class KernelMemory {
};

template<type KernelMemory>
class I2cKernelMemory {
	I2cKernelMemory(KernelMemory &k)
};

template I2cBus<type 
