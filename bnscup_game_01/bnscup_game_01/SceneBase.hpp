#pragma once

class SceneBase
{
   public:
	virtual ~SceneBase() = default;
	virtual void update() = 0;
	virtual void draw() const = 0;
};
