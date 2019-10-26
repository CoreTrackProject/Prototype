#pragma once

class IEditorView {

public:
	IEditorView() {}
	virtual ~IEditorView() {}

	virtual void Init()   = 0;
	virtual void Render() = 0;
	virtual void DeInit() = 0;
};