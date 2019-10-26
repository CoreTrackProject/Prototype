#pragma once

class IEditorModel {
public:
	IEditorModel() {}
	virtual ~IEditorModel() {}

	virtual void Init() = 0;
};