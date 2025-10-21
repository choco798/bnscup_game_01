#pragma once

class SceneBase
{
   public:
	virtual ~SceneBase() = default;
	virtual void update(bool enable) = 0;
	virtual void draw() const = 0;
};
