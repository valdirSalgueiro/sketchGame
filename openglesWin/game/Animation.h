#pragma once

class Animation
{
	public:
		enum AnimationType{
			Explosion,
			ExplosionAlien,
			Explosion2,
			JetclawExplosion
		};

		int x;
		int y;
		int currentFrame;
		bool active;
		AnimationType type;
};