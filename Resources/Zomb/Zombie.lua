local Sprite = { 
	w = 32;
	h = 54;

	pix = "Resources/Zomb/Zombie.png";
	anim = {
		
		{0, 0, 0.125, 0.25}; --Right
		{0, 0.75, 0.125, 1}; --Down
		{0, 0.5, 0.125, 0.75}; --Left
		{0, 0.25, 0.125, 0.5}; --Up
		
	};
	
}
return Sprite