#pragma once

class FrontendManager;

class FrontendAction {
public:
  virtual ~FrontendAction() = default;
  virtual bool execute(FrontendManager &manager) = 0;
};
