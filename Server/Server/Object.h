#pragma once

class Object
{
	// --
	// initalize
	// --
public:
	Object() = default;
	virtual ~Object() = default;

	// --
	// getter and setter
	// --
public:
	uint32 GetObjectId() const { return _objectId; }
	void SetObjectId(const uint32 objectId) { _objectId = objectId; }

private:
	uint32 _objectId{};
};
