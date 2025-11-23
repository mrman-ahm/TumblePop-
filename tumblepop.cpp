#include <iostream>
#include <fstream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <cstdlib>
#include <ctime>

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

void Sprite_gravity(char** lvl, float& offset_y, float& velocityY, bool& onGround, const float& gravity, bool& isJumping, float& terminal_Velocity, float& x, float& y, const int cell_size, int& Sprite_height, int& Sprite_width)
{
	offset_y = y;
	offset_y += velocityY;

	char bottom_left_down = lvl[(int)(offset_y + Sprite_height) / cell_size][(int)(x + Sprite_width/4 ) / cell_size];
	char bottom_right_down = lvl[(int)(offset_y  + Sprite_height) / cell_size][(int)(x + Sprite_width - Sprite_width/4) / cell_size];
	char bottom_mid_down = lvl[(int)(offset_y + Sprite_height) / cell_size][(int)(x + Sprite_width / 2) / cell_size];

	if ((bottom_left_down == '#' || bottom_mid_down == '#' || bottom_right_down == '#')&&velocityY>0)
	{
		onGround = true; 
	}
	else
	{
		y = offset_y;
		onGround = false;
	}

	if (!onGround)
	{
		velocityY += gravity;
		if (velocityY >= terminal_Velocity) velocityY = terminal_Velocity;
	}
	else
	{
		isJumping=false;
		velocityY = 0;
	}
}

void Collision(float& x,float& y, float& offset_x, float& offset_y, float& velocityY, bool& isJumping, bool& onGround, float& speed,char& bottom_left,char& left_mid,char&  top_left,char&  top_right, char& right_mid, char& bottom_right, bool& up_colllide, bool& left_collide, bool& right_collide, char** lvl, int& Pheight, int& Pwidth, const int cell_size){
	left_collide=false;
	right_collide=false;
	// note: y+offset_y and x+offset_x are the new positions if the movement happens
	bottom_left= lvl[(int)(y+offset_y + Pheight) / cell_size][(int)(x+offset_x+Pwidth/4) / cell_size];
	left_mid = lvl[(int)(y+offset_y + Pheight/2) / cell_size][(int)(x+offset_x+Pwidth/4) / cell_size];
	top_left = lvl[(int)(y+offset_y) / cell_size][(int)(x+offset_x+Pwidth/4) / cell_size];
	top_right = lvl[(int)(y+offset_y) / cell_size][(int)(x+offset_x+ Pwidth- Pwidth/4) / cell_size];
	right_mid = lvl[(int)(y+offset_y + Pheight/2) / cell_size][(int)(x+offset_x + Pwidth - Pwidth/4) / cell_size];
	bottom_right = lvl[(int)(y+offset_y + Pheight) / cell_size][(int)(x+offset_x + Pwidth - Pwidth/4) / cell_size];
	
	//LIMITS OTHER THAN BOTTOM LEFT AND RIGHT ARE USELESS FOR NOW BUT I STILL CREATED THEM FOR FUTURE USE
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
}

void movement_player(Event& ev, float& x, float& y, float& offset_x, float& offset_y, float& velocityY, float& speed, float jumpStrength, bool& isJumping, bool& onGround, bool& direction){
	
	offset_x=0;
	offset_y=0;

	if(Keyboard::isKeyPressed(Keyboard::Left)){
		offset_x=-speed;	
		direction=true;
	}
	if(Keyboard::isKeyPressed(Keyboard::Right)){
		offset_x=speed;
		direction=false;
	}
	if(Keyboard::isKeyPressed(Keyboard::Up)&&onGround){
		velocityY=jumpStrength;
		offset_y+=velocityY;
		isJumping=true;
	}
	
		// UPDATING THE VALUES IN COLLISION FUNCTION
	
}
void ghost_movement(Event& ev, char** lvl, float& x, float& y, float& offset_x, float& offset_y, float& velocityY, float& speed, float jumpStrength, bool& onGround, bool& direction, int& height, int& width, const int cell_size, int& GhostState){
	
	srand(time(0));

	GhostState=rand()%360; // change states randomly after few frames
	if(GhostState<100){
		GhostState=1; // looking around
	}
	else{
		GhostState=0; // normal movement
	}
	
	if(GhostState==1){
		// it will look around for some time and do nothing 
		return;
	}
	//but if is moving, this block will work
	offset_x=0;
	offset_y=0;
	
	if(direction==true){
		offset_x=-speed;
	}
	else{
		offset_x=speed;
	}

	char next_block_left=lvl[(int)(y+offset_y+height)/cell_size+1][(int)(x+offset_x)/cell_size]; // checking for end of platform
	char next_block_right=lvl[(int)(y+offset_y+height)/cell_size+1][(int)(x+offset_x+width)/cell_size];
	char bottom_left= lvl[(int)(y+offset_y + height) / cell_size][(int)(x+offset_x+width/4) / cell_size]; //for checking walls
	char bottom_right = lvl[(int)(y+offset_y + height) / cell_size][(int)(x+offset_x + width - width/4) / cell_size];

	if(next_block_left==' '&&direction==true){
		direction=false;
	}
	else if(next_block_right==' '&&direction==false){
		direction=true;
	}
	if(bottom_left=='#'&&direction==true){
		direction=false;
	}
	else if(bottom_right=='#'&&direction==false){
		direction=true;
	}

	// TO ADD: OCCASIONAL STOPS AND CHANGE OF DIRECTION
	
	x+=offset_x;
	y+=offset_y;
}

void animation(){

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

	//PLAYER data
	float player_x = 500;
	float player_y = 250;
	bool player_direction = true; //true is left, false is right
	float player_speed = 5;
	
	int PlayerHeight = 102;
	int PlayerWidth = 96;

	float player_offset_x = 0;
	float player_offset_y = 0;
	float player_velocityY = 0;
	
	bool up_collide = false;
	bool left_collide = false;
	bool right_collide = false;

	int PlayerState=0; //0=idle, 1=running, 2=jumping //means what the player is doing currently. I gave this just as example

	bool isJumping = false;  // Track if jumping
	bool onGround = false; //this is for player right now but new will be created for enemies later

	//GHOST data
	float ghost_x = 450; //sample values for now
	float ghost_y = 300;
	bool ghost_direction=true;
	float ghost_speed=2;

	int GhostHeight=102;
	int GhostWidth=96;

	float ghost_offset_x = 0;
	float ghost_offset_y = 0;
	float ghost_velocityY = 0;

	int GhostState=0; //0=normal, 1=looking around, 

	//EXTRA VARIABLES
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

	// gravity and jump
	const float jumpStrength = -20; // Initial jump velocity
	const float gravity = 1;  // Gravity acceleration
	float terminal_Velocity = 20;


	//PLAYER SPRITE
	Texture PlayerTexture;
	Sprite PlayerSprite;
	if(!(PlayerTexture.loadFromFile("Data/player.png"))){ cout<<"Error to load Player Texture\n"; return -1;} //Isme mene texture to upload bhi kia aur ye bhi check kia if the file loaded successfully
	PlayerSprite.setTexture(PlayerTexture);

	//	GHOST SPRITE
	Texture GhostTexture;
	Sprite GhostSprite;
	if(!(GhostTexture.loadFromFile("Assets/Arcade - Tumble Pop - Enemies - Ghost---motiononly.png"))){ cout<<"Error Loading Ghost Texture\n"; return -1;}
	GhostSprite.setTexture(GhostTexture);
	int startpoint=0;
	int frame=0;
	GhostSprite.setTextureRect(sf::IntRect(startpoint,0,48,32));
	// WOULD CREATE A SEPERATE SPAWN FUNCTION LATER

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

		// FOR PLAYER

		Sprite_gravity(lvl,player_offset_y,player_velocityY,onGround,gravity, isJumping, terminal_Velocity, player_x, player_y, cell_size, PlayerHeight, PlayerWidth);
		movement_player(ev, player_x, player_y, player_offset_x, player_offset_y, player_velocityY, player_speed, jumpStrength, isJumping, onGround, player_direction);
		Collision(player_x,player_y, player_offset_x, player_offset_y, player_velocityY, isJumping, onGround, player_speed, bottom_left, left_mid, top_left, top_right, right_mid, bottom_right, up_collide,left_collide,right_collide, lvl, PlayerHeight, PlayerWidth, cell_size);
		//Positioning (might create a separate function later)
		if(player_direction==true){
			PlayerSprite.setScale(3,3);		
			PlayerSprite.setPosition(player_x, player_y);
		}
		else{
			PlayerSprite.setScale(-3,3);		
			PlayerSprite.setPosition(player_x+PlayerWidth, player_y);
		}


		// FOR GHOST (sample movement for now)		
		frame++; 
		GhostSprite.setTextureRect(sf::IntRect(startpoint,0,48,32));

		if(frame>30){
			startpoint+=50; 
			if(startpoint>=50*4){
				startpoint=0; 
			}
			frame=0;
		}
		Sprite_gravity(lvl,ghost_offset_y,ghost_velocityY,onGround,gravity,isJumping, terminal_Velocity, ghost_x, ghost_y, cell_size, GhostHeight, GhostWidth);
		ghost_movement(ev, lvl, ghost_x, ghost_y, ghost_offset_x, ghost_offset_y, ghost_velocityY, ghost_speed, jumpStrength, onGround, ghost_direction, GhostHeight, GhostWidth, cell_size, GhostState);
		if(ghost_direction==true){
			GhostSprite.setScale(3,3);		
			GhostSprite.setPosition(ghost_x, ghost_y);
		}
		else{
			GhostSprite.setScale(-3,3);		
			GhostSprite.setPosition(ghost_x+GhostWidth, ghost_y);
		}

		window.draw(PlayerSprite); window.draw(GhostSprite);
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


