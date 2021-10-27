try:
  import pyfirmata
except:
  import pip
  pip.main(['install', 'pyfirmata'])
  import pyfirmata
finally:
  import time

port = 'COM6' # Change to your port
board = pyfirmata.ArduinoMega(port)

def _messageHandler(*args, **kwargs):
  response_string = pyfirmata.util.two_byte_iter_to_str(args)
  print(response_string)
board.add_cmd_handler(pyfirmata.STRING_DATA, _messageHandler)

it = pyfirmata.util.Iterator(board)
it.start()

######################### START YOUR CODE HERE #########################

request = "###operation###callnumber###result###"
#request = "###1###42###4711###"

while True:
    board.send_sysex(pyfirmata.STRING_DATA, pyfirmata.util.str_to_two_byte_iter(request))
    time.sleep(1)

######################### END YOUR CODE HERE #########################

board.exit()