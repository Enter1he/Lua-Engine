local Sprite = { 
	w = 32;
	h = 54;

	fmt = ".png";
	numf = 28;
	anim = {
		
		{0, 0, 0.125, 0.25, 7}; --Right
		{0, 0.75, 0.125, 1, 7}; --Down
		{0, 0.5, 0.125, 0.75, 7}; --Left
		{0, 0.25, 0.125, 0.5, 7}; --Up
		
	};
	
}
return Sprite