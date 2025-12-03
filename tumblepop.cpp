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
	
void Sprite_gravity(char** lvl, float& offset_y, float& velocityY, bool& onGround, bool& go_down, const float& gravity, bool& isJumping, float& terminal_Velocity, float& x, float& y, const int cell_size, int& Sprite_height, int& Sprite_width)
{
	int insurance=0;
	if(velocityY==0)//this insurance variable serves when the player is 'onGround'. 
		insurance=1; //In this case the variables below receive wrong values so to avoid that we add this variable over there to make sure they get no false value
	offset_y = y+ velocityY + insurance;
	if(offset_y<0){ offset_y=y; velocityY=0;} 
	
	char bottom_left_down = lvl[(int)(offset_y + Sprite_height) / cell_size][(int)(x + Sprite_width/4 ) / cell_size];
	char bottom_right_down = lvl[(int)(offset_y  + Sprite_height) / cell_size][(int)(x + Sprite_width - Sprite_width/4) / cell_size];
	char bottom_mid_down = lvl[(int)(offset_y + Sprite_height) / cell_size][(int)(x + Sprite_width / 2) / cell_size];

	if(go_down==1&&isJumping==0 && ((int)(offset_y + Sprite_height) / cell_size)<13) // checks if player wants to go down, also checks bottom limit
	{
	bottom_left_down = ' ';
	bottom_right_down = ' ';
	bottom_mid_down = ' ';
	}
	if ((bottom_left_down == '#' || bottom_mid_down == '#' || bottom_right_down == '#')&&velocityY>=0)
	{
		isJumping=false;
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

void wallCollision(float& x,float& y, float& offset_x, float& offset_y, float& velocityY, bool& isJumping, bool& onGround,float& speed,char& bottom_left,char& left_mid,char&  top_left,char&  top_right, char& right_mid, char& bottom_right, char& bottom_mid, bool& up_colllide, bool& left_collide, bool& right_collide, bool& mid_collide, char** lvl, int& Pheight, int& Pwidth, const int cell_size){
	left_collide=false;
	right_collide=false;
	mid_collide=false;
	// y+offset_y and x+offset_x are the new positions if the movement does happen
	bottom_left= lvl[(int)(y+offset_y + Pheight) / cell_size][(int)(x+offset_x+Pwidth/4) / cell_size];
	left_mid = lvl[(int)(y+offset_y + Pheight/2) / cell_size][(int)(x+offset_x+Pwidth/4) / cell_size];
	top_left = lvl[(int)(y+offset_y) / cell_size][(int)(x+offset_x+Pwidth/4) / cell_size];
	top_right = lvl[(int)(y+offset_y) / cell_size][(int)(x+offset_x+ Pwidth- Pwidth/4) / cell_size];
	right_mid = lvl[(int)(y+offset_y + Pheight/2) / cell_size][(int)(x+offset_x + Pwidth - Pwidth/4) / cell_size];
	bottom_right = lvl[(int)(y+offset_y + Pheight) / cell_size][(int)(x+offset_x + Pwidth - Pwidth/4) / cell_size];
	bottom_mid = lvl[(int)(y+offset_y+ Pheight)/cell_size][(int)(x+offset_x+Pwidth/2)/cell_size];
	
	if(bottom_left=='#'||(x+offset_x<0)){
		left_collide=true;
	}
	if(bottom_right=='#'||((x+offset_x)>1040)){
		right_collide=true;
	}
	if(bottom_mid=='#'){
		mid_collide=true;
	}

	if((right_collide||left_collide)&&!mid_collide){
		offset_x=0;
	}
	if(onGround||isJumping)
		x+=offset_x;
}

void movement_player(Event& ev, float& x, float& y, float& offset_x, float& offset_y, float& velocityY, float& speed, float jumpStrength, bool& isJumping, bool& onGround, bool& go_down, int& counter_go_down, bool& direction, int& vaccumframe, int startpoint_vaccum){
	
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
	if(Keyboard::isKeyPressed(Keyboard::Down)&&onGround){
		// Change player state
		if(Keyboard::isKeyPressed(Keyboard::X)&&onGround){
			onGround=false; go_down=1;
		}
		
	}
	else if(Keyboard::isKeyPressed(Keyboard::X)&&onGround){
		velocityY=jumpStrength;
		isJumping=true;
	}
	if(Keyboard::isKeyPressed(Keyboard::Z)){
		// GhostSprite[i].setTextureRect(sf::IntRect(startpoint_vaccum,0,48,32));
		// if(vaccumframe>5){
		// 	startpoint_vaccum+=50; 
		// 	if(startpoint_vaccum>=50*4){
		// 		startpoint[i]=0; 
		// 	}
		// 	vaccumframe=0;
		// }

	}
			// GhostSprite[i].setTextureRect(sf::IntRect(startpoint[i],0,48,32));
			// if(frame[i]>15){
			// 	startpoint[i]+=50; 
			// 	if(startpoint[i]>=50*4){
			// 		startpoint[i]=0; 
			// 	}
			// 	frame[i]=0;
			// }

	if(go_down==1){
		counter_go_down++;
		if( counter_go_down>15){ counter_go_down=0; go_down=0;} //this ensures that the go down variable is initialized only for 15 frames (enough to go down block)
	}
	
		// UPDATING THE VALUES IN COLLISION FUNCTION
	
}

void ghost_movement(Event& ev, char** lvl, int& timer, float& x, float& y, float& offset_x, float& offset_y, float& velocityY, float& speed, float jumpStrength, bool& onGround, bool& direction, int& height, int& width, const int cell_size, int& GhostState, int& Randomizer){
	
	if(timer==0)
		Randomizer=rand()%100; 
	if(Randomizer<40){
		GhostState=1; // looking around
	}
	else{
		GhostState=0; // normal movement
	}
	
	if(GhostState==1){
		timer++; if(timer>60) timer=0;
		return;  // it will look around for some time and do nothing 
	}
	//but if is moving, this block will work
	timer++; if(timer>360) timer=0;
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

	if(next_block_left==' '&&direction==true||x+offset_x<0){
		direction=false;
	}
	else if(next_block_right==' '&&direction==false||(x+offset_x>1030)){
		direction=true;
	}
	if(bottom_left=='#'&&direction==true){
		direction=false;
	}
	else if(bottom_right=='#'&&direction==false){
		direction=true;
	}

	x+=offset_x;
	y+=offset_y;
}

void jumpcheck(char** lvl, float& x, float& y, float& offset_x, float& offset_y, float& velocityY, float jumpStrength, bool& isJumping, bool& onGround, bool& go_down, const int cell_size, int& height, int& width, bool& direction)
{
	if(((y+ height) / cell_size)<12){

	char block_above1 = lvl[(int)(y) / cell_size-1][(int)(x+offset_x) / cell_size]; //for checking walls
	char block_above2 = lvl[(int)(y) / cell_size][(int)(x+offset_x) / cell_size];
	char block_below1 = lvl[(int)(y+ height) / cell_size+2][(int)(x+offset_x) / cell_size]; //for checking walls

	if((block_above1=='#'||block_above2=='#')&&rand()%3){
		isJumping=1;
		velocityY=jumpStrength;
	}
	else if(block_below1==' '){
	
		go_down=1;
		
	}
	}
}

void skeleton_movement(Event& ev, char** lvl, int& timer, float& x, float& y, float& offset_x, float& offset_y, float& velocityY, float& speed, bool& isJumping, float jumpStrength, bool& onGround, bool& direction, int& height, int& width, const int cell_size, int& SkeletonState, bool& go_down, int& Randomizer){
	if(timer==0)
		Randomizer=rand()%100; // change states randomly after few frames


	if(Randomizer<20)
	{
			SkeletonState=1; // look around
	}
	else if(Randomizer<50)
	{
		SkeletonState=2; // Jump
		
	}
	else
		SkeletonState=0; // walk

	
	if(SkeletonState==1){
		timer++; if(timer>60) timer=0;
		return;  // it will look around for some time and do nothing 
	}
	else if(SkeletonState==2){
		// timer++; if(timer>5) timer=0;
		if(onGround)
		jumpcheck(lvl, x, y, offset_x, offset_y, velocityY, jumpStrength, isJumping, onGround, go_down, cell_size, height, width, direction);
	}
	else if(SkeletonState==0&&onGround){
	timer++; if(timer>360) timer=0;
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
	char next_block_left1=lvl[(int)(y+offset_y)/cell_size+1][(int)(x+offset_x)/cell_size-1]; // checking for end of platform
	char next_block_left2=lvl[(int)(y+offset_y)/cell_size+1][(int)(x+offset_x)/cell_size-2]; // checking for end of platform
	char next_block_left3=lvl[(int)(y+offset_y)/cell_size+1][(int)(x+offset_x)/cell_size-3]; // checking for end of platform
	char next_block_right1=lvl[(int)(y+offset_y)/cell_size+1][(int)(x+offset_x+width)/cell_size+1];
	char next_block_right2=lvl[(int)(y+offset_y)/cell_size+1][(int)(x+offset_x+width)/cell_size+2];
	char next_block_right3=lvl[(int)(y+offset_y)/cell_size+1][(int)(x+offset_x+width)/cell_size+3];

	if((next_block_left==' '&&direction==true||x+offset_x<0)){
		if(next_block_left1==' '&&next_block_left2==' '&&next_block_left3==' '&&rand()%3==0)
			{
				isJumping=1; velocityY=jumpStrength;
			}
		else
			direction=false;
	}
	else if(next_block_right==' '&&direction==false||(x+offset_x>1030)){
		if(next_block_right1==' '&&next_block_right2==' '&&next_block_right3==' '&&rand()%3==0)
			{
				isJumping=1; velocityY=jumpStrength;
			}
		else
			direction=true;
	}
	if(bottom_left=='#'&&direction==true){
		direction=false;
	}
	else if(bottom_right=='#'&&direction==false){
		direction=true;
	}

	// TO ADD: OCCASIONAL STOPS AND CHANGE OF DIRECTION
	
	// y+=offset_y;
	}
	if(isJumping)
		x+=offset_x*2;
	else
		x+=offset_x;
}

int spawn(char** lvl, int spawncandidates[100][2], const int height, const int width){
	int x=0,y=0;
	for(int i=2; i<height; i++)
	{
		for(int j=1; j<width-1; j++)
		{
			if(lvl[i][j]=='#'&&lvl[i-2][j]==' '&&lvl[i-1][j]==' ')
			{
				spawncandidates[x][0]=i-2;
				spawncandidates[x++][1]=j;
			}
		}
	}
	return x;

	
}
void Collision(Sprite& PlayerSprite, bool& invincibility, int& lives, float& player_x, float& player_y, int p_height, int p_width, float bot_x, float bot_y, int b_height, int b_width){
	if(invincibility==0){	
		if(player_x<=bot_x&&player_y<=bot_y){
			if((bot_x-player_x)<p_width&&(bot_y-player_y)<p_height){
				player_x=100; player_y=640; invincibility=1; lives--;
			}
		}
		else if(player_x>bot_x && player_y<bot_y){
			if((player_x-bot_x)<b_width && (bot_y-player_y)<p_height){
				player_x=100; player_y=640;invincibility=1; lives--;
			}
		}

		if(player_x<bot_x && player_y>bot_y){
			if((bot_x-player_x)<p_width && (player_y-bot_y)<b_height){
				player_x=100; player_y=640;invincibility=1;lives--;
			}
		}

		if(player_x>=bot_x && player_y>=bot_y){
			if((player_x-bot_x)<b_width && (player_y-bot_y)<b_height){
				player_x=100; player_y=640;invincibility=1;lives--;
			}
		}
	}
}

void GhostSet(RenderWindow& window, char** lvl, Event& ev, Sprite GhostSprite[8], int frame[8], int ghosttimer[8], float ghost_x[8], float ghost_y[8], float ghost_offset_x[8] ,float ghost_offset_y[8], float ghost_velocityY[8], float ghost_speed, const float jumpStrength, bool& onGround, bool ghost_direction[8], int& GhostHeight, int& GhostWidth, const int cell_size, int GhostState[8], float& terminal_Velocity, const float gravity, int startpoint[8], int Randomizer[8])
{
for(int i=0; i<8; i++){
			bool dummy_onGround=0; bool dummy_isJumping=0; bool dummy_go_down=0;
			frame[i]++; 
			GhostSprite[i].setTextureRect(sf::IntRect(startpoint[i],0,48,32));
			if(frame[i]>15){
				startpoint[i]+=50; 
				if(startpoint[i]>=50*4){
					startpoint[i]=0; 
				}
				frame[i]=0;
			}
			Sprite_gravity(lvl,ghost_offset_y[i],ghost_velocityY[i],dummy_onGround, dummy_go_down, gravity, dummy_isJumping, terminal_Velocity, ghost_x[i], ghost_y[i], cell_size, GhostHeight, GhostWidth);
			ghost_movement(ev, lvl, ghosttimer[i], ghost_x[i], ghost_y[i], ghost_offset_x[i], ghost_offset_y[i], ghost_velocityY[i], ghost_speed, jumpStrength, dummy_onGround, ghost_direction[i], GhostHeight, GhostWidth, cell_size, GhostState[i], Randomizer[i]);
			if(ghost_direction[i]==true){
				GhostSprite[i].setScale(3,3);		
				GhostSprite[i].setPosition(ghost_x[i], ghost_y[i]);
			}
			else
			{
				GhostSprite[i].setScale(-3,3);		
				GhostSprite[i].setPosition(ghost_x[i]+GhostWidth, ghost_y[i]);
			}
			window.draw(GhostSprite[i]);
		}
}
		
void SkeletonSet(RenderWindow& window,char** lvl, Event& ev, Sprite SkeletonSprite[4], int frame[4], int skeletontimer[4], int counter_go_down[4], float skeleton_x[4], float skeleton_y[4], float skeleton_offset_x[4] , float skeleton_offset_y[4], float skeleton_velocityY[4], float skeleton_speed, const float jumpStrength, bool isJumping[4],  bool onGround[4], bool go_down[4],bool skeleton_direction[4], int& SkeletonHeight, int&  SkeletonWidth, const int  cell_size, int SkeletonState[4], float& terminal_Velocity , const float gravity, int startpoint[4], int Randomizer[4])
{
	for(int i = 0; i < 4; i++){
	frame[i]++; 
	SkeletonSprite[i].setTextureRect(sf::IntRect(startpoint[i], 0, 32, 48));
	if(frame[i] > 15){
		startpoint[i] += 34; 
		if(startpoint[i] >= 34 * 4){
			startpoint[i] = 0; 
		}
		frame[i] = 0;
	}
	skeleton_movement(ev,lvl,skeletontimer[i],skeleton_x[i],skeleton_y[i],skeleton_offset_x[i],skeleton_offset_y[i],skeleton_velocityY[i],skeleton_speed, isJumping[i],jumpStrength,onGround[i],skeleton_direction[i],SkeletonHeight,SkeletonWidth,cell_size,SkeletonState[i], go_down[i], Randomizer[i]);
	if(go_down[i]==1){
		counter_go_down[i]++;
		if( counter_go_down[i]>15){ counter_go_down[i]=0; go_down[i]=0;} //this ensures that the go down variable is initialized only for 15 frames (enough to go down block)
	}
	Sprite_gravity(lvl,skeleton_offset_y[i],skeleton_velocityY[i],onGround[i],go_down[i],gravity, isJumping[i],terminal_Velocity,skeleton_x[i],skeleton_y[i],cell_size,SkeletonHeight,SkeletonWidth);
	if(skeleton_direction[i] == true){
		SkeletonSprite[i].setScale(3, 3);
		SkeletonSprite[i].setPosition(skeleton_x[i], skeleton_y[i]-40);
	}
	else {
		SkeletonSprite[i].setScale(-3, 3);
		SkeletonSprite[i].setPosition(skeleton_x[i] + SkeletonWidth, skeleton_y[i]-40);
	}
	window.draw(SkeletonSprite[i]);
}
}

int main()
{
	srand(time(0));

	RenderWindow window(VideoMode(screen_x, screen_y), "Tumble-POP", Style::Resize);
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);
	//level specifics
	const int cell_size = 64;
	const int height = 14; const int width = 18;
	char** lvl;

	//level and background textures and sprites
	Texture bgTex;
	Sprite bgSprite;
	Texture blockTexture;
	Sprite blockSprite;

	bgTex.loadFromFile("Data/bg.png"); 	bgSprite.setTexture(bgTex); bgSprite.setPosition(0,0);

	blockTexture.loadFromFile("Data/block1.png"); blockSprite.setTexture(blockTexture);

	//Music initialisation
	Music lvlMusic;
	lvlMusic.openFromFile("Data/mus.ogg");	lvlMusic.setVolume(20);	lvlMusic.play();	lvlMusic.setLoop(true);

	//PLAYER data
	float player_x = 80;	float player_y = 640;
	bool player_direction = true; //true is left, false is right
	float player_speed = 5; int lives=3; int vaccumframe=0; int startpoint_vaccum=0;
	
	int PlayerHeight = 102;	int PlayerWidth = 96;
	bool invincibility=0; int invincibilitytimer=0;
	float player_offset_x = 0;	float player_offset_y = 0;
	float player_velocityY = 0;
	
	bool up_collide = false;	bool left_collide = false;	bool right_collide = false; 
	bool mid_collide = false;

	int PlayerState=0; //0=idle, 1=running, 2=jumping //means what the player is doing currently. I gave this just as example

	bool isJumping = false;  // Track if jumping
	bool onGround = false; //this is for player right now but new will be created for enemies later
	bool go_down=0;
	int counter_go_down=0;
	//GHOST data
	float ghost_x[8] = {450};	float ghost_y[8] = {300};
	bool ghost_direction[8]={true};
	float ghost_speed=2;

	int GhostHeight=102;	int GhostWidth=96;

	float ghost_offset_x[8] = {0};	float ghost_offset_y[8] = {0};
	float ghost_velocityY[8] = {0};
	int ghost_frame[8]={0};
	int ghosttimer[8]={0};
	int ghost_startpoint[8]={0};
	int GhostState[8]={0}; //0=normal, 1=looking around,
	int ghost_randomizer[8]={0};
	//Skeleton data
	float skeleton_x[4] = {450};	float skeleton_y[4] = {300};
	bool skeleton_direction[4]={true};
	float skeleton_speed=2;

	int SkeletonHeight=102;	int SkeletonWidth=96;

	float skeleton_offset_x[4] = {0};	float skeleton_offset_y[4] = {0};
	float skeleton_velocityY[4] = {0};
	int skeleton_frame[4]={0};
	int skeletontimer[4]={0};
	int skeleton_startpoint[4]={0};
	int SkeletonState[4]={0}; //0=normal, 1=looking around,
	bool skeleton_onGround[4]={0};
	bool skeleton_isJumping[4]={0};
	bool skeleton_go_down[4]={0};
	int skeleton_counter_go_down[4]={0};
	int skeleton_randomizer[4]={0};
	
	

	//EXTRA VARIABLES
	char top_left = '\0';	char top_right = '\0';	char top_mid = '\0';
	char left_mid = '\0';	char right_mid = '\0';
	char bottom_left = '\0';	char bottom_right = '\0';	char bottom_mid = '\0';
	char bottom_left_down = '\0';	char bottom_right_down = '\0';	char bottom_mid_down = '\0';
	char top_right_up = '\0';	char top_mid_up = '\0';	char top_left_up = '\0';

	// gravity and jump
	const float jumpStrength = -20; // Initial jump velocity
	const float gravity = 1;  // Gravity acceleration
	float terminal_Velocity = 20;


	//PLAYER SPRITE
	Texture PlayerTexture;	Sprite PlayerSprite; Texture VaccumText; Sprite Vaccum;
	if(!(PlayerTexture.loadFromFile("Data/player.png"))){ cout<<"Error to load Player Texture\n"; return -1;} //Isme mene texture to upload bhi kia aur ye bhi check kia if the file loaded successfully
	if(!(VaccumText.loadFromFile("Data/Vaccum.png"))){ cout<<"Error to load Vaccuum\n"; return -1;}
	PlayerSprite.setTexture(PlayerTexture); 	Vaccum.setTexture(VaccumText); 

	//	GHOST SPRITE
	Texture GhostTexture;	Sprite GhostSprite[8];
	if(!(GhostTexture.loadFromFile("Data/Arcade - Tumble Pop - Enemies - Ghost---motiononly.png"))){ cout<<"Error Loading Ghost Texture\n"; return -1;}
	for(int i=0;i<8; i++){
		GhostSprite[i].setTexture(GhostTexture);
	}
	Texture SkeletonTexture;	Sprite SkeletonSprite[4];
	if(!(SkeletonTexture.loadFromFile("Data/Arcade - Tumble Pop - Enemies - Skeleton--temp.png"))){ cout<<"Error Loading Ghost Texture\n"; return -1;}
	for(int i=0;i<4; i++){
		SkeletonSprite[i].setTexture(SkeletonTexture);
	}
	
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
		lvl[i][j] = ' ';   // for empty spacesd
		}
	}
	lvl[3][6]='#';
	lvl[4][9]='#';lvl[4][10]='#';lvl[4][11]='#'; lvl[4][12]='#';lvl[4][7]='#';lvl[4][8]='#';
	lvl[6][0]='#';lvl[6][1]='#';lvl[6][2]='#';lvl[6][3]='#';lvl[6][17]='#';lvl[6][16]='#';lvl[6][15]='#';lvl[6][14]='#';
	lvl[8][5]='#';lvl[8][6]='#';lvl[8][7]='#';lvl[8][8]='#';lvl[8][9]='#';lvl[8][10]='#';lvl[8][11]='#';lvl[8][12]='#'; lvl[11][8]='#';lvl[11][9]='#';lvl[11][10]='#';lvl[11][6]='#';lvl[11][7]='#';
	for(int i=0; i<18; i++){
		lvl[13][i]='#';
	}

	int spawncandidates[100][2];
	int candidatesavailable=spawn(lvl, spawncandidates, height, width);
	for(int i=0; i<8; i++){
		int random_coordinate=rand()%(candidatesavailable);
		ghost_y[i]=spawncandidates[random_coordinate][0]*64;
		ghost_x[i]=spawncandidates[random_coordinate][1]*64;
	}
	for(int i=0; i<4; i++){
		int random_coordinate=rand()%(candidatesavailable);
		skeleton_y[i]=spawncandidates[random_coordinate][0]*64;
		skeleton_x[i]=spawncandidates[random_coordinate][1]*64;
	}
	Event ev;
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

		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			window.close();
		}
		window.clear();
		display_level(window, lvl, bgTex, bgSprite, blockTexture, blockSprite, height, width, cell_size);

		// FOR PLAYER
		Sprite_gravity(lvl,player_offset_y,player_velocityY,onGround, go_down, gravity, isJumping, terminal_Velocity, player_x, player_y, cell_size, PlayerHeight, PlayerWidth);
		movement_player(ev, player_x, player_y, player_offset_x, player_offset_y, player_velocityY, player_speed, jumpStrength, isJumping, onGround, go_down, counter_go_down, player_direction, vaccumframe, startpoint_vaccum);
		wallCollision(player_x,player_y, player_offset_x, player_offset_y, player_velocityY, isJumping, onGround, player_speed, bottom_left, left_mid, top_left, top_right, right_mid, bottom_right, bottom_mid, up_collide,left_collide,right_collide, mid_collide, lvl, PlayerHeight, PlayerWidth, cell_size);
		if(player_direction==true){		PlayerSprite.setScale(3,3);	PlayerSprite.setPosition(player_x, player_y+2);	}
		else{	PlayerSprite.setScale(-3,3); PlayerSprite.setPosition(player_x+PlayerWidth, player_y+2);	}
		window.draw(PlayerSprite); 
		//Ghost
		GhostSet(window, lvl,ev,  GhostSprite,  ghost_frame,  ghosttimer,  ghost_x,  ghost_y,  ghost_offset_x , ghost_offset_y, ghost_velocityY, ghost_speed, jumpStrength, onGround,ghost_direction, GhostHeight, GhostWidth, cell_size, GhostState, terminal_Velocity , gravity, ghost_startpoint, ghost_randomizer);
		//Skeleton
		SkeletonSet(window, lvl,ev,  SkeletonSprite,  skeleton_frame,  skeletontimer, skeleton_counter_go_down,  skeleton_x,  skeleton_y,  skeleton_offset_x , skeleton_offset_y, skeleton_velocityY, skeleton_speed, jumpStrength, skeleton_isJumping, skeleton_onGround,  skeleton_go_down, skeleton_direction, SkeletonHeight, SkeletonWidth, cell_size, SkeletonState, terminal_Velocity , gravity, skeleton_startpoint, skeleton_randomizer);
		for(int i = 0; i<4; i++)
			Collision( PlayerSprite, invincibility, lives, player_x,  player_y,  PlayerHeight, PlayerWidth,  skeleton_x[i],  skeleton_y[i],  SkeletonHeight,  SkeletonWidth);
		for(int i = 0; i<8; i++)
			Collision( PlayerSprite, invincibility, lives, player_x,  player_y,  PlayerHeight, PlayerWidth,  ghost_x[i],  ghost_y[i],  GhostHeight,  GhostWidth);
		
		if(invincibility==1){
			invincibilitytimer++;
			if(invincibilitytimer>100){
				invincibility=0; invincibilitytimer=0;}
		}
		if(lives==0)	
			exit(0);
		// Despawn(player_x, player_y, ghost_x, ghost_y);
		// Despawn(player_x, player_y, skeleton_x, skeleton_y);
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


