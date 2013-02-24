#pragma once

template<class T> class ConstantBuffer;

class Camera
{
public:
	Camera(float fov, float aspectRatio);
	virtual ~Camera();

	virtual void Update(float timeSinceLastUpdate) {}

	void UpdateGPUBuffer();
	void ActivateCameraConstantBufferForAllShader(unsigned int bufferSlotIndex);

	const SimpleMath::Matrix& GetProjectionMatrix() const;
	const SimpleMath::Matrix& GetViewMatrix() const;
	const SimpleMath::Matrix& GetViewProjectionMatrix() const;

	const SimpleMath::Vector3& GetPosition() const { return _position; }
	const SimpleMath::Vector3& GetUp() const { return _up; }
	const SimpleMath::Vector3& GetViewDir() const { return _viewDir; }

protected:
	void UpdateMatrices();

	SimpleMath::Vector3 _position;
	SimpleMath::Vector3 _up;
	SimpleMath::Vector3 _viewDir;

private:

	struct Constants
	{
		SimpleMath::Matrix _viewProjectionMatrix;
		
		SimpleMath::Matrix _projectionMatrix;
		SimpleMath::Matrix _viewMatrix;
		SimpleMath::Vector3 _position;
		float padding;
	};
	std::unique_ptr<ConstantBuffer<Constants>> _constantBuffer;
};