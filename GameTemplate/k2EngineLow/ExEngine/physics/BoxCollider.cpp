
#include "k2EngineLowPreCompile.h"
#include "ExEngine.h"
#include "Physics.h"
#include "BoxCollider.h"




void BoxCollider::Create( const Vector3& size )
{
	shape = std::make_unique<btBoxShape>(btVector3(size.x*0.5f, size.y*0.5f, size.z*0.5f));
}
