import struct

targetFile = input("Target File?: ")
theFile = open(targetFile, 'rb')

while(theFile):
    #Grab the number
    characterNumber = struct.unpack('Q',theFile.read(8))

    #Get the binary string for it
    characterBinaryString = '{0:064b}'.format(characterNumber[0])

    #Reverse the string to get it in the right order
    characterBinaryString = characterBinaryString[::-1]
    
    #Convert it back to a number
    characterNumber = int (characterBinaryString, 2)

    #Create a hex string from it.
    characterHexString = '{0:#0{1}x}'.format(characterNumber,18)

    print (characterHexString[0:2] +
           characterHexString[16:18] +
           characterHexString[14:16] +
           characterHexString[12:14] +
           characterHexString[10:12] +
           characterHexString[8:10] +
           characterHexString[6:8] +
           characterHexString[4:6] +
           characterHexString[2:4] +
           ',')



    
