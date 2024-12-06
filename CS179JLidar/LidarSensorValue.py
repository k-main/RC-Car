# Version 1 of python code is done???
# Version 2 will get actual data from the ESP32 instead of random values
# Version 3 will incorporate the movement of x and y coordinates into it ... hopefully
import pygame
import math
import random

# star pygame up
pygame.init()

# Screen dimensions and settings
WIDTH, HEIGHT = 1000, 1000
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Whatever the Lidar Sees now you see")

# Colors (may add more to make it cooler)
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
BLUE = (0, 0, 255)
RED = (255, 0, 0)

#Replace this when u actually get the data and not just random data
def genLidarData():
    angles = [i * 1.8 for i in range(200)]  # 2This gives us angles between 0 and 360 degrees in order and provides 1.8 degree increments
    distances = [random.uniform(450, 600) for _ in angles]  # Random distances in terms of pixel distance between 250 and 500 degrees of the def CAR
    return angles, distances #returns both values for ease of use of one generation function

def main():
    # setups the bool fro running and clock in order to update the screen
    running = True
    clock = pygame.time.Clock()

    while running: #basic begins of pygame initialization
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False


        screen.fill(WHITE) # make entire background white

        # Draw def not irobot roomba robot at the center of the screen
        origin_x, origin_y = WIDTH // 2, HEIGHT // 2
        pygame.draw.circle(screen, RED, (origin_x, origin_y), 5)

        # Generate defintely real lidar data
        angles, distances = genLidarData()

        for angle, distance in zip(angles, distances): # This will plot the dots/data points for a stable car https://www.geeksforgeeks.org/zip-in-python/
            # Convert polar coordinates to Cartesian coordinates we need a radius of the area
            if distance <= 500: # just to be certain that their is something there
                angle_rad = math.radians(angle)
                x = origin_x + distance * math.cos(angle_rad)
                y = origin_y - distance * math.sin(angle_rad)  # Pygames y values are inverted and I HATE IT!!

                
                pygame.draw.circle(screen, BLUE, (int(x), int(y)), 3) # Draw the point distance/angle

        #HOW DO I UPDATE THE DISPLAY :( https://stackoverflow.com/questions/29314987/difference-between-pygame-display-update-and-pygame-display-flip Love this person
        pygame.display.flip()

        # Cap the frame rate
        clock.tick(30)  # 30 FPS

    pygame.quit()


#if __name__ == "__main__": # This is not necessary but included it, becuase I did a similar thing in highscolol 
main()
