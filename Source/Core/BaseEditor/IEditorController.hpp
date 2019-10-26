#pragma once

class IEditorController {
public:
	IEditorController() {}
	virtual ~IEditorController() {}

	virtual void Init()  = 0;
	virtual void DeInit() = 0;

	virtual void Render() = 0;

};
