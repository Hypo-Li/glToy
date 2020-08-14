#pragma once
namespace ys
{
	enum BodyPosture
	{
		//վ��
		Body_Posture_Stand,

		//�¶�
		Body_Posture_Squat,

		//�Ե�
		Body_Posture_Crawl,

		//վ��->�¶�
		Body_Posture_Stand_to_Squat,

		//�¶�->�Ե�
		Body_Posture_Squat_to_Crawl,

		//վ��->�Ե�
		Body_Posture_Stand_to_Crawl,

		//�Ե�->�¶�
		Body_Posture_Crawl_to_Squat,

		//�¶�->վ��
		Body_Posture_Squat_to_Stand,

		//�Ե�->վ��
		Body_Posture_Crawl_to_Stand,

		//��Ծ
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