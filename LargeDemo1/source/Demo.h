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
	std::unique_ptr<class Window> _window;
	std::unique_ptr<class Cube> _cube;
};