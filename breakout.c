/**  CS50 edX pset4 submission by Jacob Sherman
* breakout.c
* Summer 2014
**/


// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// math library to round floats and other math functions
#include <math.h> 

// Stanford Portable Library
#include "gevents.h"
#include "gobjects.h"
#include "gwindow.h"

// height and width of game's window in pixels
#define HEIGHT 600
#define WIDTH 400

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// radius of ball in pixels
#define RADIUS 10

// lives
#define LIVES 3

// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
void updateScoreboard(GWindow window, GLabel label, int points);
GObject detectCollision(GWindow window, GOval ball);

int main(void)
{
    // seed pseudorandom number generator
    srand48(time(NULL));

    // instantiate window
    GWindow window = newGWindow(WIDTH, HEIGHT);

    // instantiate bricks   
    initBricks(window);

    // instantiate ball, centered in middle of window
    GOval ball = initBall(window);

    // instantiate paddle, centered at bottom of window
    GRect paddle = initPaddle(window);

    // instantiate scoreboard, centered in middle of window, just above ball
    GLabel label = initScoreboard(window);

    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;

    // number of points initially
    int points = 0;

    // wait for click before starting
    waitForClick();

    double velocityX = drand48() * 5;
    double velocityY = drand48() * 4;
    float ballRadius = getWidth(ball) / 2;
    // keep playing until game over
    while (lives > 0 && bricks > 0)
    {
        // TODO
        GEvent event = getNextEvent(MOUSE_EVENT);  
        GObject objectHit = detectCollision(window, ball); 
        /// ball movement
        move(ball, velocityX, velocityY);
            
        // bounce off right edge of window
        if (getX(ball) + getWidth(ball) >= getWidth(window))
        {
            velocityX = -velocityX;
        }
        // bounce off left edge of window
        else if (getX(ball) <= 0)
        {
            velocityX = -velocityX;
        }
        //  bounce off top edge
        else if ( (getY(ball) + (RADIUS *2)) <= 0)
        {
            velocityY = -velocityY;
        }
          
        // bounce off bottom of window
        else if (getY(ball) + getWidth(ball) >= HEIGHT)
        {
             lives--;
             pause(100);
             setLocation(ball, (WIDTH - (RADIUS * 2))/2, (HEIGHT - (RADIUS * 2))/2);
             if (lives == 0)
             {
                     GLabel loser = newGLabel("Loser!!");
                     setFont(loser, "Courier-64");
                     setColor(loser, "RED");    
                     double xOver = (WIDTH - getWidth(loser)) / 2;
                     double yOver = (HEIGHT - getHeight(loser)) / 3;
                     setLocation(loser, xOver, yOver);
                     add(window, loser);
                     break;
              }   
          }
          // check if object hit
          else if (objectHit != NULL)
          {
          // check if hit object is paddle and ball falling
          if (objectHit == paddle && (velocityY > 0 ) )
          {
               velocityY = -velocityY;
               // call setLocation to ensure no collision on next loop
               setLocation(ball, getX(ball), getY(ball) -2);
          }
          // check if hit bricks 
          else if ((strcmp(getType(objectHit), "GRect") == 0) && (objectHit != paddle) ) 
          {
              velocityY = -velocityY;
              // remove hit brick
              removeGWindow(window, objectHit);
              // set location off brick to prevent multiple collisions
              setLocation(ball,getX(ball), getY(ball) + 10);
              // add points, subtract bricks and update scoreboard
              points++;
              bricks--;
              updateScoreboard(window, label, points);
              if (bricks == 0)
              {
                    GLabel winner = newGLabel("You Won");
                    setFont(winner, "Courier-64");
                    setColor(winner, "GREEN");
                    double xOver = (WIDTH - getWidth(winner)) / 2;
                    double yOver = (HEIGHT - getHeight(winner)) / 3;
                    setLocation(winner, xOver, yOver);
                    add(window, winner);
               }
           }
           }        
           pause(10);
               
           ///  paddle movement code     
           double yPaddle = getY(paddle);
           double xPaddle = getX(paddle);
           float paddleObjWidth = getWidth(paddle);
           float paddleStop = (WIDTH - paddleObjWidth);
                              
           if ( (event != NULL) && getEventType(event) == MOUSE_MOVED)
           {
               double x = getX(event);
           if (x <= paddleStop)
           {             
                setLocation(paddle, x, yPaddle);
           }
           else
           {
                setLocation(paddle, paddleStop, yPaddle);
           }
           }
           /// end paddle movement code  
    }   // end while loop


    // wait for click before exiting
    waitForClick();

    // game over
    closeGWindow(window);
    return 0;
}

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{
    // TODO
    // initial margin space
    float leftMargin = (float)WIDTH * .01;     // need to clean up numbers formating
    float rightMargin = (float)WIDTH * .01;    // floats correct ??
    float rowY = 0;                         // sets Y position of bricks
    // calculate brick size
    int brickWidth = WIDTH/COLS;
    int brickHeight = HEIGHT/50;
    float gap = (float)brickWidth * .05;
    brickWidth = brickWidth - gap - (rightMargin/COLS);
    
    //char array of colors
    char color1[4] = "RED";  // auto null zero
    char color2[7] = "ORANGE";
    char color3[7] = "YELLOW";
    char color4[6] = "GREEN";
    char color5[5] = "BLUE";
    
    // 2D array of color char arrays ie strings
    char* colors[] = { color1, color2, color3,  color4,  color5 };
    
    for (int rowNum = 0; rowNum < 5; rowNum++)
    {
        // need to change color or bricks here
            // one row of bricks
            for (int j = 0, leftSide = leftMargin; j < 10; j++)
            {

              GRect rect = newGRect(leftSide, rowY, brickWidth, brickHeight);
              setFilled(rect, true);
              setColor(rect, colors[rowNum]);
              add(window, rect);
              
              leftSide = leftSide + (gap + brickWidth);
            }
        rowY = rowY + ( round((float)brickHeight * 1.25));
            
    }  // end row for loop
}

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    // TODO
 //  taken out because realized there is a constant 
 //   int ballRadius = round( (float)WIDTH * .05);
 //   GOval ball = newGOval( (WIDTH- ballRadius)/2, (HEIGHT- ballRadius)/2, ballRadius, ballRadius);
    GOval ball = newGOval( (WIDTH- (RADIUS * 2))/2, (HEIGHT- (RADIUS * 2))/2, (RADIUS * 2), (RADIUS * 2));
    setFilled(ball, true);
    setColor(ball, "BLACK");;
    add(window, ball);
    
    return ball;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{
    // TODO
    int bottomMargin =  round( (float)HEIGHT * .05);
    int paddleWidth = round((float)WIDTH * .175 );
    int paddleHeight = round((float)HEIGHT * .02 );
    
    GRect paddle = newGRect( (WIDTH- paddleWidth)/2, HEIGHT - bottomMargin, paddleWidth, paddleHeight);
    setFilled(paddle, true);
    setColor(paddle, "BLACK");
    add(window, paddle);
    return paddle;
}

/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
    // TODO
    
    GLabel label = newGLabel("0");
    setFont(label, "Courier-64");
    setColor(label, "C0C0C0");

    // center label in window
    double x = (WIDTH - getWidth(label)) / 2;
    double y = (HEIGHT - getHeight(label)) / 2;
    setLocation(label, x, y);
    add(window, label);
    return label;
}

/**
 * Updates scoreboard's label, keeping it centered in window.
 */
void updateScoreboard(GWindow window, GLabel label, int points)
{
    // update label
    char s[12];
    sprintf(s, "%i", points);
    setLabel(label, s);

    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
}

/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GOval ball)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // no collision
    return NULL;
}
