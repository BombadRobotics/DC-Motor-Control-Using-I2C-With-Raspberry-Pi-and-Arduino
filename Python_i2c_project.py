import smbus
import time
import RPi.GPIO as GPIO 

# Setting the bus to its appropriate RPi version, this version being 3.7.6
bus = smbus.SMBus(1)

# Setting GPIO pinmode as BCM (the numbers after the pin numbers)
GPIO.setmode(GPIO.BCM)

# Setting the address used for Arduino communication
address = 0x04

# Initializing GPIO 18 as an output
GPIO.setup(18, GPIO.OUT) 

# Setting the PWM at GPIO 18 to operate at 100Hz, increasing this didn't seem 
# to make any noticeable difference, so I just left it at 100.
p=GPIO.PWM(18,100)
# Initializing the pwm to start at 0
p.start(0)

# Setting the state variable which is used to determine whether to display a
# message when the system is 'off' or 'on'
# The system is initially 'off'
state = 0
try:
    #Setting this to run forever in a loop unless interrupted
    while True:
        # This reads any incoming data from the arduino as a 'data block', 
        # essentially importing it as an array/list
        data = bus.read_i2c_block_data(address,1) #(address,bytes/length) 
        time.sleep(.15) #delay to give time for the bus to properly read without error
        
        #When the system is on, incoming data is > 0 and state = 1
        if data[0] != 0:
            if state == 1:
                print('System is Enabled')
                # Having the state value change upon exit ensures that the 
                # message only prints once upon a change in the incoming data
                # of data = 0, and data > 0
                state = 0 
            
            datamod=data[0]-1   #this takes care of the 1-102 offset to be 0-101 
            if datamod>100:     #this sets any value greater than 100 to be 100
                datamod=100
            #takes the incoming/edited value and sets that as the pwm of the motor
            p.ChangeDutyCycle(datamod)
            
            #this prints the value accordingly to the terminal
            print('Motor Speed %: ',(datamod))
            
            
        #When the system is off, incoming data is 0 and state = 0 
        elif data[0] == 0:
            if state == 0:
                print('System is Disabled')
                state = 1
                #this turns off the PWM, thus turning off the motor
                p.ChangeDutyCycle(0)
except KeyboardInterrupt:
    p.stop()            #stops the PWM
    GPIO.cleanup()      #Cleans up or resets the pins upon the interrupt exit