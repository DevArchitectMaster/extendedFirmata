import pyfirmata
import time


board = pyfirmata.ArduinoMega('COM5')
it = pyfirmata.util.Iterator(board)
it.start()

board.digital[10].mode = pyfirmata.INPUT

while True:
    sw = board.digital[10].read()
    #print(sw)
    if sw is True:
        board.digital[13].write(1)
    else:
        board.digital[13].write(0)
    time.sleep(0.1)

'''
while True:
    board.digital[13].write(1)
    time.sleep(1)
    board.digital[13].write(0)
    time.sleep(1)
'''