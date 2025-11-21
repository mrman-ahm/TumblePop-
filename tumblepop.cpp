#include <iostream>
#include <fstream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>

using namespace sf;
using namespace std;

int screen_x = 1136;
int screen_y = 896;

void display_level(RenderWindow& window, char**lvl, Texture& bgTex,Sprite& bgSprite,Texture& blockTexture,Sprite& blockSprite, const int height, const int width, const int cell_size)
{
	window.draw(bgSprite);

	for (int i = 0; i < height; i += 1)
	{
		for (int j = 0; j < width; j += 1)
		{

			if (lvl[i][j] == '#')
			{
				blockSprite.setPosition(j * cell_size, i * cell_size);
				window.draw(blockSprite);
			}
		}
	}

}

void player_gravity(char** lvl, float& offset_y, float& velocityY, bool& onGround, const float& gravity, float& terminal_Velocity, float& player_x, float& player_y, const int cell_size, int& Pheight, int& Pwidth)
{
	offset_y = player_y;

	offset_y += velocityY;

	char bottom_left_down = lvl[(int)(offset_y + Pheight) / cell_size][(int)(player_x + Pwidth/4 ) / cell_size];
	char bottom_right_down = lvl[(int)(offset_y  + Pheight) / cell_size][(int)(player_x + Pwidth - Pwidth/4) / cell_size];
	char bottom_mid_down = lvl[(int)(offset_y + Pheight) / cell_size][(int)(player_x + Pwidth / 2) / cell_size];

	if ((bottom_left_down == '#' || bottom_mid_down == '#' || bottom_right_down == '#')&&velocityY>0)
	{
		onGround = true;
	}
	else
	{
		player_y = offset_y;
		onGround = false;
	}

	if (!onGround)
	{
		velocityY += gravity;
		if (velocityY >= terminal_Velocity) velocityY = terminal_Velocity;
	}

	else
	{
		velocityY = 0;
	}
}

void Collision(float& x,float& y, float& offset_x, float& offset_y, float& velocityY,float& speed,char& bottom_left,char& left_mid,char&  top_left,char&  top_right, char& right_mid, char& bottom_right, bool& up_colllide, bool& left_collide, bool& right_collide, char** lvl, int& Pheight, int& Pwidth, const int cell_size){
	left_collide=false;
	right_collide=false;
	bottom_left= lvl[(int)(y+offset_y + Pheight) / cell_size][(int)(x+offset_x+Pwidth/4) / cell_size];
	left_mid = lvl[(int)(y+offset_y + Pheight/2) / cell_size][(int)(x+offset_x+Pwidth/4) / cell_size];
	top_left = lvl[(int)(y+offset_y) / cell_size][(int)(x+offset_x+Pwidth/4) / cell_size];
	top_right = lvl[(int)(y+offset_y) / cell_size][(int)(x+offset_x+ Pwidth- Pwidth/4) / cell_size];
	right_mid = lvl[(int)(y+offset_y + Pheight/2) / cell_size][(int)(x+offset_x + Pwidth - Pwidth/4) / cell_size];
	bottom_right = lvl[(int)(y+offset_y + Pheight) / cell_size][(int)(x+offset_x + Pwidth - Pwidth/4) / cell_size];
	
	//REST ARE USELESS FOR NOW
	if(bottom_left=='#'){
		left_collide=true;
	}
	if(bottom_right=='#'){
		right_collide=true;
	}



	if(right_collide||left_collide){
		offset_x=0;
	}

	x+=offset_x;
	y+=offset_y;
	

}

void movement(Event& ev, float& x, float& y, float& offset_x, float& offset_y, float& velocityY, float& speed, float jumpStrength, bool& onGround){
	offset_x=0;
	offset_y=0;
	if(Keyboard::isKeyPressed(Keyboard::Left)){
		offset_x=-speed;		
	}
	if(Keyboard::isKeyPressed(Keyboard::Right)){
		offset_x=speed;
	}
		

	if (ev.type == Event::KeyPressed)
	{
		if(Keyboard::isKeyPressed(Keyboard::Up)&&onGround){
		velocityY=jumpStrength;
		offset_y+=velocityY;
	

			}
		}


		// UPDATING THE VALUES IN COLLISION FUNCTION
	
}


int main()
{

	RenderWindow window(VideoMode(screen_x, screen_y), "Tumble-POP", Style::Resize);
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);

	//level specifics
	const int cell_size = 64;
	const int height = 14;
	const int width = 18;
	char** lvl;

	//level and background textures and sprites
	Texture bgTex;
	Sprite bgSprite;
	Texture blockTexture;
	Sprite blockSprite;

	bgTex.loadFromFile("Data/bg.png");
	bgSprite.setTexture(bgTex);
	bgSprite.setPosition(0,0);

	blockTexture.loadFromFile("Data/block1.png");
	blockSprite.setTexture(blockTexture);

	//Music initialisation
	Music lvlMusic;

	lvlMusic.openFromFile("Data/mus.ogg");
	lvlMusic.setVolume(20);
	lvlMusic.play();
	lvlMusic.setLoop(true);

	//player data
	float player_x = 500;
	float player_y = 250;

	float speed = 5;

	const float jumpStrength = -20; // Initial jump velocity
	const float gravity = 1;  // Gravity acceleration

	bool isJumping = false;  // Track if jumping

	bool up_collide = false;
	bool left_collide = false;
	bool right_collide = false;

	Texture PlayerTexture;
	Sprite PlayerSprite;

	bool onGround = false;

	float offset_x = 0;
	float offset_y = 0;
	float velocityY = 0;

	float terminal_Velocity = 20;

	int PlayerHeight = 102;
	int PlayerWidth = 96;

	bool up_button = false;

	char top_left = '\0';
	char top_right = '\0';
	char top_mid = '\0';

	char left_mid = '\0';
	char right_mid = '\0';

	char bottom_left = '\0';
	char bottom_right = '\0';
	char bottom_mid = '\0';

	char bottom_left_down = '\0';
	char bottom_right_down = '\0';
	char bottom_mid_down = '\0';

	char top_right_up = '\0';
	char top_mid_up = '\0';
	char top_left_up = '\0';

	PlayerTexture.loadFromFile("Data/player.png");
	PlayerSprite.setTexture(PlayerTexture);
	PlayerSprite.setScale(-3,3);
	//	FLIPPED


	//creating level array
	lvl = new char* [height];
	for (int i = 0; i < height; i += 1)
	{
		lvl[i] = new char[width]; 
	}

	for (int i = 0; i < height; i++)
	{
    	for (int j = 0; j < width; j++)
    	{
      	  lvl[i][j] = ' ';   // for empty spaces
    	}
	}

	lvl[6][0]='#';lvl[6][1]='#';lvl[6][2]='#';lvl[6][3]='#';lvl[6][17]='#';lvl[6][16]='#';lvl[6][15]='#';lvl[6][14]='#';
	lvl[9][5]='#';
	lvl[10][5]='#';lvl[10][6]='#';lvl[11][7]='#';lvl[11][8]='#';lvl[11][9]='#';lvl[11][10]='#';lvl[11][11]='#';lvl[11][12]='#';

	for(int i=0; i<18; i++){
		lvl[0][i]='#'; lvl[13][i]='#';
	}
	Event ev;
	//main loop
	while (window.isOpen())
	{

		while (window.pollEvent(ev))
		{
			if (ev.type == Event::Closed) 
			{
				window.close();
			}

			if (ev.type == Event::KeyPressed)
			{
			}
		}

		//presing escape to close
		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			window.close();
		}

		window.clear();

		display_level(window, lvl, bgTex, bgSprite, blockTexture, blockSprite, height, width, cell_size);
		player_gravity(lvl,offset_y,velocityY,onGround,gravity,terminal_Velocity, player_x, player_y, cell_size, PlayerHeight, PlayerWidth);
		movement(ev, player_x, player_y, offset_x, offset_y, velocityY, speed, jumpStrength, onGround);
		Collision(player_x,player_y, offset_x, offset_y, velocityY, speed, bottom_left, left_mid, top_left, top_right, right_mid, bottom_right, up_collide,left_collide,right_collide, lvl, PlayerHeight, PlayerWidth, cell_size);

		PlayerSprite.setPosition(player_x+PlayerWidth, player_y);

		window.draw(PlayerSprite);
		window.display();
}

	//stopping music and deleting level array
	lvlMusic.stop();
	for (int i = 0; i < height; i++)
	{
		delete[] lvl[i];
	}
	delete[] lvl;

	return 0;
}

