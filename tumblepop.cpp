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
	
void Sprite_gravity(char** lvl, float& offset_y, float& velocityY, bool& onGround, bool& go_down, const float& gravity, bool& isJumping, float& terminal_Velocity, float& x, float& y, const int cell_size, int& Sprite_height, int& Sprite_width, int& State)
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
		if(isJumping) 
			State=3;
		else 
			State=6;
		velocityY += gravity;
		if (velocityY >= terminal_Velocity) velocityY = terminal_Velocity;
	}
	else
	{
		isJumping=false;
		velocityY = 0;
		State=1;
	}
}

void wallCollision(float& x,float& y, float& offset_x, float& offset_y, float& velocityY, bool& isJumping, bool& onGround,float& speed,char& bottom_left,char& left_mid,char&  top_left,char&  top_right, char& right_mid, char& bottom_right, char& bottom_mid, bool& up_colllide, bool& left_collide, bool& right_collide, bool& mid_collide, char** lvl, int& Pheight, int& Pwidth, const int cell_size, int& PlayerState){
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
		if(offset_x!=0)
		PlayerState=2; // 2=Walking on ground
	else if(onGround)
		PlayerState=1;
}

void movement_player(Event& ev, Sprite& Vaccum, bool& vaccumOn, float& x, float& y, float& offset_x, float& offset_y, float& velocityY, float& speed, float jumpStrength, bool& isJumping, bool& onGround, bool& go_down, int& counter_go_down, bool& direction, int& updownvaccum, int& vaccumframe, int& startpoint_vaccum, int& PlayerState){
	
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
		PlayerState=4;
		if(Keyboard::isKeyPressed(Keyboard::X)&&onGround){
			PlayerState=6;
			onGround=false; go_down=1;
		}
		
	}
	else if(Keyboard::isKeyPressed(Keyboard::X)&&onGround){
		velocityY=jumpStrength;
		isJumping=true;
	}
	vaccumOn=0;
	if(Keyboard::isKeyPressed(Keyboard::Z)){
		vaccumOn=1;
		vaccumframe++;
		Vaccum.setTextureRect(sf::IntRect(startpoint_vaccum,0,48,32));
		if(vaccumframe>5){
			startpoint_vaccum+=48; 
			if(startpoint_vaccum>=48*5){
				vaccumframe=0; startpoint_vaccum=0;
			}
		}
	}

	if(Keyboard::isKeyPressed(Keyboard::W)){
		up
	}
			
	if(go_down==1){
		counter_go_down++;
		if( counter_go_down>15){ counter_go_down=0; go_down=0;} //this ensures that the go down variable is initialized only for 15 frames (enough to go down block)
	}
	
		// UPDATING THE VALUES IN COLLISION FUNCTION
	
}

void Suck(int& BotState, int bot_type, int bot_index, int& nextinbag, int bag_bot_type[], int bagbot_x[], int bagbot_y[], int bag_bot_index[], int vaccumpositionX, int vaccumpositionY, int direction, float& bot_x, float& bot_y, int suck_distance){
	int DistanceX, DistanceY;
	if(vaccumpositionX>bot_x) DistanceX=vaccumpositionX-bot_x;
	else DistanceX=bot_x-vaccumpositionX;
	if(vaccumpositionY>=bot_y) DistanceY=vaccumpositionY-bot_y;
	else DistanceY=bot_y-vaccumpositionY;
	
	if(direction){
		if((DistanceX<=suck_distance)&&vaccumpositionY>=bot_y&&(DistanceY<50)){
			BotState=3;
			bot_x+=7;
		}
		else if((DistanceX<=suck_distance)&&vaccumpositionY<bot_y&&(DistanceY<50)){
			BotState=3;
			bot_x+=7;
		}
	}
	else{
		if((DistanceX<=suck_distance)&&vaccumpositionY>=bot_y&&(DistanceY<50)){
			bot_x-=7;
			BotState=3;
		}
		else if((DistanceX<=suck_distance)&&vaccumpositionY<bot_y&&(DistanceY<50)){
			bot_x-=7;
			BotState=3;
		}
	}

	if(BotState==3&&DistanceY<60){
		if(nextinbag < 3){
			BotState=4;
			bag_bot_type[nextinbag]=bot_type;
			bag_bot_index[nextinbag]=bot_index;
			bagbot_x[nextinbag]=bot_x=0;
			bagbot_y[nextinbag]=bot_y=0;
			nextinbag++;
		}
	}
}

void Release(int& nextinbag, int bag_bot_type[], int bag_bot_index[], int GhostState[], int SkeletonState[], float ghost_throw_velocity_x[], float ghost_throw_velocity_y[], float skeleton_throw_velocity_x[], float skeleton_throw_velocity_y[], bool player_direction){
	if (Keyboard::isKeyPressed(Keyboard::S) && nextinbag > 0)
	{
		int last_type = bag_bot_type[nextinbag-1];
		int last_index = bag_bot_index[nextinbag-1];
		if(last_type == 0) {
			GhostState[last_index] = 5;
			ghost_throw_velocity_x[last_index] = player_direction ? -15 : 15;  // Throw opposite to player direction
			ghost_throw_velocity_y[last_index] = -15;  // Throw upward
		}
		else if(last_type == 1) {
			SkeletonState[last_index] = 5;
			skeleton_throw_velocity_x[last_index] = player_direction ? -15 : 15;  // Throw opposite to player direction
			skeleton_throw_velocity_y[last_index] = -15;  // Throw upward
		}
		nextinbag--;
	}
	else if (Keyboard::isKeyPressed(Keyboard::A))
	{
		for(int i=0; i<nextinbag; i++){
			if(bag_bot_type[i] == 0) {
				GhostState[bag_bot_index[i]] = 5;
				ghost_throw_velocity_x[bag_bot_index[i]] = player_direction ? -15 : 15;
				ghost_throw_velocity_y[bag_bot_index[i]] = -15;
			}
			else if(bag_bot_type[i] == 1) {
				SkeletonState[bag_bot_index[i]] = 5;
				skeleton_throw_velocity_x[bag_bot_index[i]] = player_direction ? -15 : 15;
				skeleton_throw_velocity_y[bag_bot_index[i]] = -15;
			}
		}
		nextinbag=0;
	}

}

// ================================================================================

int ThrownCollision(float& bot_x, float& bot_y, float& throw_velocity_x, float& throw_velocity_y, char** lvl, const int cell_size, int bot_height, int bot_width, const int height, const int width, const float gravity){
	// Check collision with walls and apply rebound
	// bot_x, bot_y: current position of thrown bot
	// throw_velocity_x, throw_velocity_y: velocity of thrown bot
	// Returns: 1 if collision happened (bot hit floor), 0 otherwise
	
	// Apply gravity to thrown bot
	// throw_velocity_y += gravity;
	
	// Update position
	// Check if position is out of bounds (hit floor)
	if(bot_y + bot_height >= height * cell_size) {
		return 1; // Hit floor, stop throwing
	}
	
	// Clamp to screen bounds on X axis with rebound
	if(bot_x < 0) {
		bot_x = 0;
		throw_velocity_x *= -0.8; // Rebound with energy loss
	}
	if(bot_x + bot_width > width * cell_size) {
		bot_x = width * cell_size - bot_width;
		throw_velocity_x *= -0.8; // Rebound with energy loss
	}
	
	// Check collision with environment blocks ('#')
	// Check multiple points on the bot's body for accurate collision
	int bot_cell_left = (int)(bot_x + bot_width / 4) / cell_size;
	int bot_cell_right = (int)(bot_x + bot_width - bot_width / 4) / cell_size;
	int bot_cell_mid = (int)(bot_x + bot_width / 2) / cell_size;
	
	int bot_cell_top = (int)(bot_y) / cell_size;
	int bot_cell_bottom = (int)(bot_y + bot_height) / cell_size;
	int bot_cell_mid_y = (int)(bot_y + bot_height / 2) / cell_size;
	
	// Ensure indices are within bounds
	if(bot_cell_left < 0) bot_cell_left = 0;
	if(bot_cell_left >= width) bot_cell_left = width - 1;
	if(bot_cell_right >= width) bot_cell_right = width - 1;
	if(bot_cell_mid >= width) bot_cell_mid = width - 1;
	if(bot_cell_top < 0) bot_cell_top = 0;
	if(bot_cell_top >= height) bot_cell_top = height - 1;
	if(bot_cell_bottom >= height) bot_cell_bottom = height - 1;
	if(bot_cell_mid_y >= height) bot_cell_mid_y = height - 1;
	
	// Collision from bottom (floor/platform)
	if(lvl[bot_cell_bottom][bot_cell_left] == '#' || 
	   lvl[bot_cell_bottom][bot_cell_mid] == '#' || 
	   lvl[bot_cell_bottom][bot_cell_right] == '#'||(bot_cell_bottom>850)) {
		throw_velocity_x*=0.8;
		throw_velocity_y *= -0.8; // Rebound with energy loss
		bot_y = bot_cell_bottom * cell_size - bot_height;
	}
	
	if(throw_velocity_y > -2 && throw_velocity_y < 2 && throw_velocity_x>-4 && throw_velocity_x<4) {
		return 1; // Stop throwing if velocity is too low
	}
	// Collision from top (ceiling)
	if(bot_y <= 0|| (lvl[bot_cell_top][bot_cell_left] == '#' || 
	   lvl[bot_cell_top][bot_cell_mid] == '#' || 
	   lvl[bot_cell_top][bot_cell_right] == '#')) {
		throw_velocity_y *= -0.8; // Rebound with energy loss
		throw_velocity_x*=0.8;
		bot_y = (bot_cell_top + 1) * cell_size;
	}
	
	// Collision from left
	if(bot_x<= 0 && lvl[bot_cell_mid_y][bot_cell_left] == '#') {
		throw_velocity_x *= -0.5; // Rebound with energy loss
		bot_x = (bot_cell_left + 1) * cell_size;
	}
	
	// Collision from right
	if(bot_x+ bot_width>=width && lvl[bot_cell_mid_y][bot_cell_right] == '#') {
		throw_velocity_x *= -0.5; // Rebound with energy loss
		bot_x = bot_cell_right * cell_size - bot_width;
	}
	bot_x += throw_velocity_x;
	bot_y += throw_velocity_y;
	
	
	return 0; // No floor collision yet
}

void BeingThrown(){
	// This function is now a stub - actual throwing logic should be called from main loop
	// with proper per-bot tracking
	return;
}


void ghost_movement(Event& ev, char** lvl, int& timer, float& x, float& y, float& offset_x, float& offset_y, float& velocityY, float& speed, float jumpStrength, bool& onGround, bool& direction, int& height, int& width, const int cell_size, int& GhostState, int& Randomizer){
	
	if(timer==0)
		Randomizer=rand()%100; 
	if(Randomizer<40){
		GhostState=1; // looking around
	}
	else{
		GhostState=2; // normal movement
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
		SkeletonState=7; // Jump  //////////WHERE IS LOOK UP BEFORE JUMP
		
	}
	else
		SkeletonState=2; // walk

	
	if(SkeletonState==1){
		timer++; if(timer>60) timer=0;
		return;  // it will look around for some time and do nothing 
	}
	else if(SkeletonState==7){
		// timer++; if(timer>5) timer=0;
		if(onGround)
		jumpcheck(lvl, x, y, offset_x, offset_y, velocityY, jumpStrength, isJumping, onGround, go_down, cell_size, height, width, direction);
	}
	else if(SkeletonState==2&&onGround){
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

void b2bCollision(int& Bot1State, int& Bot2State, float& bot1_x, float& bot1_y, int b1_height, int b1_width, float& bot2_x, float& bot2_y, int b2_height, int b2_width){
		if(bot1_x<=bot2_x&&bot1_y<=bot2_y){
			if((bot2_x-bot1_x)<b1_width&&(bot2_y-bot1_y)<b1_height){
				// bot1_x=100; bot1_y=640; invincibility=1; lives--;
				if(Bot2State!=5&&Bot2State!=4){
				Bot1State=0; Bot2State=0;
				bot1_y=0; bot2_y=0;
				bot1_x=0; bot2_x=0;}
			}
		}
		else if(bot1_x>bot2_x && bot1_y<bot2_y){
			if((bot1_x-bot2_x)<b2_width && (bot2_y-bot1_y)<b1_height){
				// bot1_x=100; bot1_y=640;invincibility=1; lives--;
				if(Bot2State!=5&&Bot2State!=4){
				Bot1State=0; Bot2State=0;
				bot1_y=0; bot2_y=0;
				bot1_x=0; bot2_x=0;}
			}
		}

		if(bot1_x<bot2_x && bot1_y>bot2_y){
			if((bot2_x-bot1_x)<b1_width && (bot1_y-bot2_y)<b2_height){
				// bot1_x=100; bot1_y=640;invincibility=1;lives--;
				if(Bot2State!=5&&Bot2State!=4){
				Bot1State=0; Bot2State=0;
				bot1_y=0; bot2_y=0;
				bot1_x=0; bot2_x=0;}
			}
		}

		if(bot1_x>=bot2_x && bot1_y>=bot2_y){
			if((bot1_x-bot2_x)<b2_width && (bot1_y-bot2_y)<b2_height){
				// bot1_x =100; bot1_y=640;invincibility=1;lives--;
				if(Bot2State!=5&&Bot2State!=4){
				Bot1State=0; Bot2State=0;
				bot1_x=0; bot2_x=0;
				bot1_y=0; bot2_y=0;
			}
			}
		}
	
}

void GhostSet(RenderWindow& window, char** lvl,bool ghost_sucked[8], Event& ev, Sprite GhostSprite[8], int frame[8], int ghosttimer[8], float ghost_x[8], float ghost_y[8], float ghost_offset_x[8] ,float ghost_offset_y[8], float ghost_velocityY[8], float ghost_speed, const float jumpStrength, bool& onGround, bool ghost_direction[8], int& GhostHeight, int& GhostWidth, const int cell_size, int GhostState[8], float& terminal_Velocity, const float gravity, int startpoint[8], int Randomizer[8])
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
			if(GhostState[i]!=4&&GhostState[i]!=0){
				if(GhostState[i]!=5){
			Sprite_gravity(lvl,ghost_offset_y[i],ghost_velocityY[i],dummy_onGround, dummy_go_down, gravity, dummy_isJumping, terminal_Velocity, ghost_x[i], ghost_y[i], cell_size, GhostHeight, GhostWidth, GhostState[i]);
			ghost_movement(ev, lvl, ghosttimer[i], ghost_x[i], ghost_y[i], ghost_offset_x[i], ghost_offset_y[i], ghost_velocityY[i], ghost_speed, jumpStrength, dummy_onGround, ghost_direction[i], GhostHeight, GhostWidth, cell_size, GhostState[i], Randomizer[i]);
				}
			if(ghost_direction[i]==true){
				GhostSprite[i].setScale(3,3);		
				GhostSprite[i].setPosition(ghost_x[i], ghost_y[i]);
			}
			else
			{
				GhostSprite[i].setScale(-3,3);		
				GhostSprite[i].setPosition(ghost_x[i]+GhostWidth, ghost_y[i]);
			}
			// std::cout<<GhostState[i];
				window.draw(GhostSprite[i]);
		}
			else
				GhostSprite[i].setPosition(0,0);
		}
}
		
void SkeletonSet(RenderWindow& window,char** lvl, bool skeleton_sucked[4], Event& ev, Sprite SkeletonSprite[4], int frame[4], int skeletontimer[4], int counter_go_down[4], float skeleton_x[4], float skeleton_y[4], float skeleton_offset_x[4] , float skeleton_offset_y[4], float skeleton_velocityY[4], float skeleton_speed, const float jumpStrength, bool isJumping[4],  bool onGround[4], bool go_down[4],bool skeleton_direction[4], int& SkeletonHeight, int&  SkeletonWidth, const int  cell_size, int SkeletonState[4], float& terminal_Velocity , const float gravity, int startpoint[4], int Randomizer[4])
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
	if(SkeletonState[i]!=4&&SkeletonState[i]!=0){
		if(SkeletonState[i]!=5){
	skeleton_movement(ev,lvl,skeletontimer[i],skeleton_x[i],skeleton_y[i],skeleton_offset_x[i],skeleton_offset_y[i],skeleton_velocityY[i],skeleton_speed, isJumping[i],jumpStrength,onGround[i],skeleton_direction[i],SkeletonHeight,SkeletonWidth,cell_size,SkeletonState[i], go_down[i], Randomizer[i]);
	if(go_down[i]==1){
		counter_go_down[i]++;
		if( counter_go_down[i]>15){ counter_go_down[i]=0; go_down[i]=0;} //this ensures that the go down variable is initialized only for 15 frames (enough to go down block)
	}
	Sprite_gravity(lvl,skeleton_offset_y[i],skeleton_velocityY[i],onGround[i],go_down[i],gravity, isJumping[i],terminal_Velocity,skeleton_x[i],skeleton_y[i],cell_size,SkeletonHeight,SkeletonWidth, SkeletonState[i]);
}
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
	else
		SkeletonSprite[i].setPosition(0,0);
		
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
	int updownvaccum;
	float player_speed = 5; int lives=100;

	int PlayerHeight = 102;	int PlayerWidth = 96;
	float player_offset_x = 0;	float player_offset_y = 0;
	float player_velocityY = 0;

	bool up_collide = false;	bool left_collide = false;	bool right_collide = false; 
	bool mid_collide = false;
	
	
	int sizeofbag=3;
	int vaccumframe=0; int startpoint_vaccum=0; bool vaccumOn=0;
	int vaccumpositionX=0; int vaccumpositionY=0;
	int suck_distance=48*3; int total_sucked=0;
	bool invincibility=0; int invincibilitytimer=0;
	int SCOUNTER=0;
	// Track bots in bag: store {bot_type, index} where bot_type: 0=ghost, 1=skeleton
	int bag_bot_type[sizeofbag]={-1}; 
	int bag_bot_index[sizeofbag]={-1}; 
	int bagbot_x[sizeofbag]={-1};
	int bagbot_y[sizeofbag]={-1};
	int nextinbag=0;

	int PlayerState=0; //0=idle, 1=running, 2=jumping //means what the player is doing currently. I gave this just as example

	bool isJumping = false;  // Track if jumping
	bool onGround = false; //this is for player right now but new will be created for enemies later
	bool go_down=0;
	int counter_go_down=0;
	//GHOST data
	float ghost_x[8] = {450};	float ghost_y[8] = {300};
	bool ghost_direction[8]={true}; bool ghost_sucked[8]={0};
	float ghost_speed=2;

	int GhostHeight=102;	int GhostWidth=96;

	float ghost_offset_x[8] = {0};	float ghost_offset_y[8] = {0};
	float ghost_velocityY[8] = {0};
	int ghost_frame[8]={0};
	int ghosttimer[8]={0};
	int ghost_startpoint[8]={0};
	int GhostState[8]={0}; 
	for( int i=0; i<8; i++){
		GhostState[i]=2;
	}
	int ghost_randomizer[8]={0};
	
	float ghost_throw_velocity_x[8] = {0};  // Horizontal throw velocity
	float ghost_throw_velocity_y[8] = {0};  // Vertical throw velocity
	
	//Skeleton data
	float skeleton_x[4] = {450};	float skeleton_y[4] = {300};
	bool skeleton_direction[4]={true}; bool skeleton_sucked[4]={0};
	float skeleton_speed=2;

	int SkeletonHeight=102;	int SkeletonWidth=96;

	float skeleton_offset_x[4] = {0};	float skeleton_offset_y[4] = {0};
	float skeleton_velocityY[4] = {0};
	int skeleton_frame[4]={0};
	int skeletontimer[4]={0};
	int skeleton_startpoint[4]={0};
	int SkeletonState[4]={2}; 
	for( int i=0; i<4; i++){
		SkeletonState[i]=2;
	}
	bool skeleton_onGround[4]={0};
	bool skeleton_isJumping[4]={0};
	bool skeleton_go_down[4]={0};
	int skeleton_counter_go_down[4]={0};
	int skeleton_randomizer[4]={0};
	
	float skeleton_throw_velocity_x[4] = {0};
	float skeleton_throw_velocity_y[4] = {0};
	


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
	if(!(VaccumText.loadFromFile("Data/Vaccum.png"))){ cout<<"Error to load Vaccum\n"; return -1;}
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

		if (Keyboard::isKeyPressed(Keyboard::RShift))
		{
			window.close();
		}
		window.clear();
		display_level(window, lvl, bgTex, bgSprite, blockTexture, blockSprite, height, width, cell_size);

		// FOR PLAYER

		Sprite_gravity(lvl,player_offset_y,player_velocityY,onGround, go_down, gravity, isJumping, terminal_Velocity, player_x, player_y, cell_size, PlayerHeight, PlayerWidth, PlayerState);
		movement_player(ev, Vaccum, vaccumOn, player_x, player_y, player_offset_x, player_offset_y, player_velocityY, player_speed, jumpStrength, isJumping, onGround, go_down, counter_go_down, player_direction,updownvaccum, vaccumframe, startpoint_vaccum, PlayerState);
		wallCollision(player_x,player_y, player_offset_x, player_offset_y, player_velocityY, isJumping, onGround, player_speed, bottom_left, left_mid, top_left, top_right, right_mid, bottom_right, bottom_mid, up_collide,left_collide,right_collide, mid_collide, lvl, PlayerHeight, PlayerWidth, cell_size, PlayerState);
		// Release(nextinbag, bag_bot_type, bag_bot_index, GhostState, SkeletonState);
		if(player_direction==true){
				vaccumpositionX=player_x-48*3; vaccumpositionY= player_y+12;
				PlayerSprite.setScale(3,3);	PlayerSprite.setPosition(player_x, player_y+2);
				Vaccum.setScale(3,3); Vaccum.setPosition(vaccumpositionX, vaccumpositionY);
				}
		else{
			vaccumpositionX=player_x+48*3; vaccumpositionY= player_y+12;			
			PlayerSprite.setScale(-3,3); PlayerSprite.setPosition(player_x+PlayerWidth, player_y+2);
			Vaccum.setScale(-3,3);  Vaccum.setPosition(vaccumpositionX+PlayerWidth, vaccumpositionY);	
		}
		window.draw(PlayerSprite); 	

		for(int i = 0; i<4; i++){
			if(SkeletonState[i]==5) {
				for(int j=0; j<4; j++) //j!=i
				// if(i!=j)
					b2bCollision(SkeletonState[i], SkeletonState[j], skeleton_x[i], skeleton_y[i], SkeletonHeight, SkeletonWidth, skeleton_x[j], skeleton_y[j], SkeletonHeight, SkeletonWidth);
				for(int j=0;j<8;j++)
				// if(i!=j)
					b2bCollision(SkeletonState[i], GhostState[j], skeleton_x[i], skeleton_y[i], SkeletonHeight, SkeletonWidth, ghost_x[j], ghost_y[j], GhostHeight, GhostWidth);

				// Being thrown - handle collision and physics
				int collision_result = ThrownCollision(
					skeleton_x[i], skeleton_y[i],
					skeleton_throw_velocity_x[i], skeleton_throw_velocity_y[i],
					lvl, cell_size, SkeletonHeight, SkeletonWidth, height, width, gravity
				);
				if(collision_result == 1) {
					SkeletonState[i] = 0;  // Return to normal state
					skeleton_throw_velocity_x[i] = 0;
					skeleton_throw_velocity_y[i] = 0;
				}
			}
			else {
				// Not being thrown - normal collision check
				if(SkeletonState[i]==4||SkeletonState[i]==0)
					SkeletonSprite[i].setPosition(0,0);
				else if(SkeletonState[i]!=3)
				Collision(PlayerSprite, invincibility, lives, player_x, player_y, PlayerHeight, PlayerWidth, skeleton_x[i], skeleton_y[i], SkeletonHeight, SkeletonWidth);
				// Only suck if not in bag (state != 4)
				if(vaccumOn && SkeletonState[i] != 4&&SkeletonState[i]!=0)
					Suck(SkeletonState[i], 1, i, nextinbag, bag_bot_type, bagbot_x, bagbot_y, bag_bot_index, vaccumpositionX, vaccumpositionY, player_direction, skeleton_x[i], skeleton_y[i], suck_distance);
			}
		}
		for(int i = 0; i<8; i++){
			if(GhostState[i]==5) {

				for(int j=0; j<4; j++) //j!=i
				if(i!=j)
					b2bCollision(GhostState[i], SkeletonState[j], ghost_x[i], ghost_y[i], GhostHeight, GhostWidth, skeleton_x[j], skeleton_y[j], SkeletonHeight, SkeletonWidth);
				for(int j=0;j<8;j++)
				if(i!=j)
					b2bCollision(GhostState[i], GhostState[j], ghost_x[i], ghost_y[i], GhostHeight, GhostWidth, ghost_x[j], ghost_y[j], GhostHeight, GhostWidth);

				// Being thrown - handle collision and physics
				int collision_result = ThrownCollision(
					ghost_x[i], ghost_y[i],
					ghost_throw_velocity_x[i], ghost_throw_velocity_y[i],
					lvl, cell_size, GhostHeight, GhostWidth, height, width, gravity
				);
				if(collision_result == 1) {
					GhostState[i] = 0;  // Return to normal state
					ghost_throw_velocity_x[i] = 0;
					ghost_throw_velocity_y[i] = 0;
				}
			}
			else {
				// Not being thrown - normal collision check
				if(GhostState[i]==4||GhostState[i]==0)
					GhostSprite[i].setPosition(0,0);
				else if(GhostState[i]!=3)
				Collision(PlayerSprite, invincibility, lives, player_x, player_y, PlayerHeight, PlayerWidth, ghost_x[i], ghost_y[i], GhostHeight, GhostWidth);
				// Only suck if not in bag (state != 4)
				if(vaccumOn && GhostState[i] != 4&&GhostState[i]!=0)
					Suck(GhostState[i], 0, i, nextinbag, bag_bot_type, bagbot_x, bagbot_y, bag_bot_index, vaccumpositionX, vaccumpositionY, player_direction, ghost_x[i], ghost_y[i], suck_distance);
			}
		}
		//Ghost
		GhostSet(window, lvl, ghost_sucked, ev, GhostSprite, ghost_frame, ghosttimer, ghost_x, ghost_y, ghost_offset_x, ghost_offset_y, ghost_velocityY, ghost_speed, jumpStrength, onGround, ghost_direction, GhostHeight, GhostWidth, cell_size, GhostState, terminal_Velocity, gravity, ghost_startpoint, ghost_randomizer);

		SkeletonSet(window, lvl, skeleton_sucked, ev, SkeletonSprite, skeleton_frame, skeletontimer, skeleton_counter_go_down, skeleton_x, skeleton_y, skeleton_offset_x, skeleton_offset_y, skeleton_velocityY, skeleton_speed, jumpStrength, skeleton_isJumping, skeleton_onGround, skeleton_go_down, skeleton_direction, SkeletonHeight, SkeletonWidth, cell_size, SkeletonState, terminal_Velocity, gravity, skeleton_startpoint, skeleton_randomizer);

				//////////////////////////////////////////////////////////////////////////////////////////////////////////
		if (Keyboard::isKeyPressed(Keyboard::S) && nextinbag > 0 )
	{
		SCOUNTER++;
		if(SCOUNTER>10) SCOUNTER=0;
		if(SCOUNTER==1){
		int last_type = bag_bot_type[nextinbag-1];
		int last_index = bag_bot_index[nextinbag-1];
		
		bagbot_x[nextinbag-1]=vaccumpositionX;
		bagbot_y[nextinbag-1]=vaccumpositionY;
		if(last_type == 0) {
			GhostState[last_index] = 5;
			ghost_throw_velocity_x[last_index] = player_direction ? -15 : 15;  // Throw opposite to player direction
			ghost_throw_velocity_y[last_index] = -5;  // Throw upward
			ghost_x[last_index]=vaccumpositionX;
			ghost_y[last_index]=vaccumpositionY-10;
		}
		else if(last_type == 1) {
			SkeletonState[last_index] = 5;
			skeleton_throw_velocity_x[last_index] = player_direction ? -15 : 15;  // Throw opposite to player direction
			skeleton_throw_velocity_y[last_index] = -5;  // Throw upward
			skeleton_x[last_index]=vaccumpositionX;
			skeleton_y[last_index]=vaccumpositionY-10;
		}
		nextinbag--;
	}
	}
	else if (Keyboard::isKeyPressed(Keyboard::A))
	{
		for(int i=0; i<nextinbag; i++){
		bagbot_x[i]=vaccumpositionX;
		bagbot_y[i]=vaccumpositionY;

			if(bag_bot_type[i] == 0) {
				GhostState[bag_bot_index[i]] = 5;
				ghost_throw_velocity_x[bag_bot_index[i]] = player_direction ? -15 : 15;
				// ghost_throw_velocity_y[bag_bot_index[i]] = -15;
			}
			else if(bag_bot_type[i] == 1) {
				SkeletonState[bag_bot_index[i]] = 5;
				skeleton_throw_velocity_x[bag_bot_index[i]] = player_direction ? -15 : 15;
				// skeleton_throw_velocity_y[bag_bot_index[i]] = -15;
			}
		}
		nextinbag=0;
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

		// Death Detection

		if(invincibility==1){
			invincibilitytimer++;
			if(invincibilitytimer>100){
				invincibility=0; invincibilitytimer=0;
			}
		}
		if(vaccumOn)
			window.draw(Vaccum); 
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


