#pragma once
namespace ys
{
	enum BodyPosture
	{
		//Õ¾Á¢
		Body_Posture_Stand,

		//ÏÂ¶×
		Body_Posture_Squat,

		//ÎÔµ¹
		Body_Posture_Crawl,

		//Õ¾Á¢->ÏÂ¶×
		Body_Posture_Stand_to_Squat,

		//ÏÂ¶×->ÎÔµ¹
		Body_Posture_Squat_to_Crawl,

		//Õ¾Á¢->ÎÔµ¹
		Body_Posture_Stand_to_Crawl,

		//ÎÔµ¹->ÏÂ¶×
		Body_Posture_Crawl_to_Squat,

		//ÏÂ¶×->Õ¾Á¢
		Body_Posture_Squat_to_Stand,

		//ÎÔµ¹->Õ¾Á¢
		Body_Posture_Crawl_to_Stand,

		//ÌøÔ¾
		Body_Posture_Jump,
	};

	enum MoveDirection
	{
		Move_Direction_None,

		Move_Direction_Front,

		Move_Direction_Back,

		Move_Direction_Left,

		Move_Direction_Right,
	};
}