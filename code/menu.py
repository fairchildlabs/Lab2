import pygame
import button
import mmap
import os
import struct
from multiprocessing import shared_memory



pygame.init()






shm = shared_memory.SharedMemory(name='scootd_shared.mem')

if(shm is None):
    print ("Error attaching shared memory... exiting")
    quit()




#def set_shared(ival)
def get_shared_ival():
    shared_int = struct.unpack('i', shm.buf[:4])[0]
    print (shared_int)
    return shared_int
        
def set_shared_ival(ival):
    shared_int = shm.buf
    struct.pack_into('i', shared_int, 0, ival)
    nval = get_shared_ival()
    print("PYTHON:nval %d ival = %d\n" % (nval, ival))




ival = get_shared_ival()

#if ival == 0:
#    print("IVAL == 0 quit")
#    quit()


#create game window
SCREEN_WIDTH = 800
SCREEN_HEIGHT = 600

screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
pygame.display.set_caption("Scootcam")

#game variables
game_paused = True
menu_state = "stop"

#define fonts
font = pygame.font.SysFont("arialblack", 40)

#define colours
TEXT_COL = (255, 255, 255)

#load button images
record_img = pygame.image.load("images/rec-button.png").convert_alpha()
stop_img = pygame.image.load("images/stop-button.png").convert_alpha()


#create button instances
record_button = button.Button(250, 125, record_img, .75)
stop_button = button.Button(250, 125, stop_img, .75)

def draw_text(text, font, text_col, x, y):
  img = font.render(text, True, text_col)
  screen.blit(img, (x, y))

#game loop
run = True
while run:

  screen.fill((255, 255, 255))

  if menu_state == "stop":
    #draw pause screen buttons
     if record_button.draw(screen):
        menu_state = "record"
        set_shared_ival(11)
  else: 
     if stop_button.draw(screen):
        menu_state = "stop"
        set_shared_ival(22)


 
  #event handler
  for event in pygame.event.get():
     if event.type == pygame.KEYDOWN:
       if event.key == pygame.K_SPACE:
         run = False
       if event.key == pygame.K_ESCAPE:
         run = False
     if event.type == pygame.QUIT:
       run = False

  pygame.display.update()


shm.close()

pygame.quit()