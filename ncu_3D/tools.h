 #ifndef  TOOLS_H__
#define TOOLS_H__
/**
	created by esli 2013/11/7
	content: common tool class
	description: namespace util includes a set of common tools,like some simple functions of drawing.
	so you can add your common function in it. 
*/
namespace util{
	static const double PI = 3.1415926f;
	struct Point{
		double x;double y;double z;
		Point():x(0.0f),y(0.0f),z(0.0f){}
	};
	class shape
	{
	public:
		static void drawarc(Point center,double radius,Point vector,double start_angle,double end_angle,double delta_angle);
		static void drawcircle(Point center,double radius,Point vector,double delta_angle);
		static void drawpie(Point center,double radius,Point vector,double start_angle,double end_angle,double delta_angle);
	};
	class render
	{
	public:
		static void circle_render(Point center,GLfloat radius,GLfloat delta_angle,GLuint texture);
	};
}
#endif/*end*/