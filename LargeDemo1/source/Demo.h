template<class T> class ConstantBuffer;

class Demo
{
public:
	Demo();
	~Demo();

	/// perform heavy weighted init stuff
	/// creating window, setting up device...
	bool Initialize(HINSTANCE hInstance);

	/// runts the main loop
	void RunMainLoop();

	void Draw(float timeSinceLastUpdate);
	void Update(float timeSinceLastUpdate);

private:
	static const unsigned int CONSTANT_BUFFER_INDEX_CAMERA = 5;
	static const unsigned int CONSTANT_BUFFER_INDEX_SCREEN = 6;

	float _passedTimeSinceStart;
	unsigned int _frameNumber;

	std::unique_ptr<class Window> _window;

	std::unique_ptr<class Cube> _cube;
	std::unique_ptr<class Terrain> _terrain;

	std::unique_ptr<class Camera> _camera;

	struct ScreenConstants
	{
		SimpleMath::Vector2 ScreenSize;
		float AspectRatio;
	};
	std::unique_ptr<ConstantBuffer<ScreenConstants>> _screenConstants;

	std::unique_ptr<class FontRenderer> _fontRenderer;
	std::unique_ptr<class FontSheet> _font0;
};