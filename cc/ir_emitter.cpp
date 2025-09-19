#include "ir_emitter.hpp"

Symbol *IREmitter::getSymbol(const std::string &name) {
  for (int i = symbols.size() - 1; i >= 0; --i) {
    if (symbols[i].name == name) {
      return &symbols[i];
    }
  }
  return nullptr;
}

std::any IREmitter::visit(cparse::FunctionDefinition &node) {
  auto func_type = llvm::FunctionType::get(builder.getInt16Ty(), false);
  auto function = llvm::Function::Create(
      func_type, llvm::Function::ExternalLinkage, node.name, &module);
  auto entry_bb = llvm::BasicBlock::Create(context, "", function);
  builder.SetInsertPoint(entry_bb);

  symbols.push_back({node.name, function, func_type});

  visit(*node.body);

  return std::any();
}

std::any IREmitter::visit(cparse::Block &node) {
  for (auto &decl : node.declarations) {
    visit(*decl);
  }
  for (auto &stmt : node.statements) {
    visit(*stmt);
  }

  for (auto &decl : node.declarations) {
    symbols.pop_back();
  }

  return std::any();
}

std::any IREmitter::visit(cparse::Declaration &node) {
  if (getSymbol(node.name)) {
    throw std::runtime_error("Error: redeclaration of variable " + node.name);
  }

  auto alloca = builder.CreateAlloca(builder.getInt16Ty(), nullptr, node.name);
  symbols.push_back({node.name, alloca, builder.getInt16Ty()});

  if (node.initializer) {
    auto init_val = std::any_cast<llvm::Value *>(visit(*node.initializer));
    builder.CreateStore(init_val, alloca);
  }

  return std::any();
}

std::any IREmitter::visit(cparse::ReturnStatement &node) {
  llvm::Value *ret_val = nullptr;
  if (node.expression) {
    ret_val = std::any_cast<llvm::Value *>(visit(*node.expression));
  } else {
    // TODO: Make this "undefined" for non-void functions
    ret_val = llvm::ConstantInt::get(builder.getInt16Ty(), 0);
  }
  builder.CreateRet(ret_val);
  return std::any();
}

std::any IREmitter::visit(cparse::ExpressionStatement &node) {
  if (node.expression) {
    visit(*node.expression);
  }
  return std::any();
}

std::any IREmitter::visit(cparse::IfStatement &node) {
  if (!node.condition || !node.then_branch) {
    throw std::runtime_error("Error: incomplete if statement");
  }

  auto cond_val = std::any_cast<llvm::Value *>(visit(*node.condition));
  cond_val = builder.CreateICmpNE(
      cond_val, llvm::ConstantInt::get(builder.getInt16Ty(), 0));

  auto function = builder.GetInsertBlock()->getParent();

  auto then_bb = llvm::BasicBlock::Create(context, "", function);
  llvm::BasicBlock *else_bb = nullptr;
  llvm::BasicBlock *merge_bb = nullptr;

  if (node.else_branch) {
    else_bb = llvm::BasicBlock::Create(context, "", function);
    merge_bb = llvm::BasicBlock::Create(context, "", function);
    builder.CreateCondBr(cond_val, then_bb, else_bb);
  } else {
    merge_bb = llvm::BasicBlock::Create(context, "", function);
    builder.CreateCondBr(cond_val, then_bb, merge_bb);
  }

  builder.SetInsertPoint(then_bb);
  visit(*node.then_branch);
  builder.CreateBr(merge_bb);

  if (node.else_branch) {
    builder.SetInsertPoint(else_bb);
    visit(*node.else_branch);
    builder.CreateBr(merge_bb);
  }
  builder.SetInsertPoint(merge_bb);

  return std::any();
}

std::any IREmitter::visit(cparse::CompoundStatement &node) {
  if (node.block) {
    visit(*node.block);
  }
  return std::any();
}

std::any IREmitter::visit(cparse::ConstantExpression &node) {
  return static_cast<llvm::Value *>(
      llvm::ConstantInt::get(builder.getInt16Ty(), node.value));
}

std::any IREmitter::visit(cparse::IdentifierExpression &node) {
  auto symbol = getSymbol(node.name);
  if (!symbol) {
    throw std::runtime_error("Error: use of undeclared variable " + node.name);
  }
  return static_cast<llvm::Value *>(
      builder.CreateLoad(symbol->type, symbol->value));
}

std::any IREmitter::visit(cparse::UnaryExpression &node) {
  if (!node.operand) {
    throw std::runtime_error("Error: unary expression missing operand");
  }

  auto operand = std::any_cast<llvm::Value *>(visit(*node.operand));

  switch (node.op) {
  case cparse::UnaryExpression::Negate:
    return static_cast<llvm::Value *>(builder.CreateNeg(operand));
  case cparse::UnaryExpression::Complement:
    return static_cast<llvm::Value *>(builder.CreateZExt(
        builder.CreateICmpEQ(operand, llvm::ConstantInt::get(builder.getInt16Ty(), 0)),
        builder.getInt16Ty()));
  default:
    throw std::runtime_error("Error: unknown unary operator");
  }
}

std::any IREmitter::visit(cparse::BinaryExpression &node) {
  if (!node.left || !node.right) {
    throw std::runtime_error("Error: binary expression missing operand(s)");
  }

  auto left = std::any_cast<llvm::Value *>(visit(*node.left));
  auto right = std::any_cast<llvm::Value *>(visit(*node.right));

  switch (node.op) {
  case cparse::BinaryExpression::Add:
    return static_cast<llvm::Value *>(builder.CreateAdd(left, right));
  case cparse::BinaryExpression::Subtract:
    return static_cast<llvm::Value *>(builder.CreateSub(left, right));
  case cparse::BinaryExpression::Multiply:
    return static_cast<llvm::Value *>(builder.CreateMul(left, right));
  case cparse::BinaryExpression::Divide:
    return static_cast<llvm::Value *>(builder.CreateSDiv(left, right));
  case cparse::BinaryExpression::Modulus:
    return static_cast<llvm::Value *>(builder.CreateSRem(left, right));
  case cparse::BinaryExpression::And: {
    // Short-circuit AND: if left is false, return 0 without evaluating right
    auto left_cond = builder.CreateICmpNE(left, llvm::ConstantInt::get(builder.getInt16Ty(), 0));
    
    auto function = builder.GetInsertBlock()->getParent();
    auto left_bb = builder.GetInsertBlock();
    auto right_bb = llvm::BasicBlock::Create(context, "", function);
    auto merge_bb = llvm::BasicBlock::Create(context, "", function);
    
    builder.CreateCondBr(left_cond, right_bb, merge_bb);
    
    // Right evaluation block
    builder.SetInsertPoint(right_bb);
    auto right_val = std::any_cast<llvm::Value *>(visit(*node.right));
    auto right_cond = builder.CreateZExt(
        builder.CreateICmpNE(right_val, llvm::ConstantInt::get(builder.getInt16Ty(), 0)),
        builder.getInt16Ty());
    builder.CreateBr(merge_bb);
    right_bb = builder.GetInsertBlock();
    
    // Merge block
    builder.SetInsertPoint(merge_bb);
    auto phi = builder.CreatePHI(builder.getInt16Ty(), 2);
    phi->addIncoming(llvm::ConstantInt::get(builder.getInt16Ty(), 0), left_bb);
    phi->addIncoming(right_cond, right_bb);
    
    return static_cast<llvm::Value *>(phi);
  }
  case cparse::BinaryExpression::Or: {
    // Short-circuit OR: if left is true, return 1 without evaluating right
    auto left_cond = builder.CreateICmpNE(left, llvm::ConstantInt::get(builder.getInt16Ty(), 0));
    
    auto function = builder.GetInsertBlock()->getParent();
    auto left_bb = builder.GetInsertBlock();
    auto right_bb = llvm::BasicBlock::Create(context, "", function);
    auto merge_bb = llvm::BasicBlock::Create(context, "", function);
    
    builder.CreateCondBr(left_cond, merge_bb, right_bb);
    
    // Right evaluation block
    builder.SetInsertPoint(right_bb);
    auto right_val = std::any_cast<llvm::Value *>(visit(*node.right));
    auto right_cond = builder.CreateZExt(
        builder.CreateICmpNE(right_val, llvm::ConstantInt::get(builder.getInt16Ty(), 0)),
        builder.getInt16Ty());
    builder.CreateBr(merge_bb);
    right_bb = builder.GetInsertBlock();
    
    // Merge block
    builder.SetInsertPoint(merge_bb);
    auto phi = builder.CreatePHI(builder.getInt16Ty(), 2);
    phi->addIncoming(llvm::ConstantInt::get(builder.getInt16Ty(), 1), left_bb);
    phi->addIncoming(right_cond, right_bb);
    
    return static_cast<llvm::Value *>(phi);
  }
  case cparse::BinaryExpression::Equal:
    return static_cast<llvm::Value *>(builder.CreateZExt(
        builder.CreateICmpEQ(left, right), builder.getInt16Ty()));
  case cparse::BinaryExpression::NotEqual:
    return static_cast<llvm::Value *>(builder.CreateZExt(
        builder.CreateICmpNE(left, right), builder.getInt16Ty()));
  case cparse::BinaryExpression::Less:
    return static_cast<llvm::Value *>(builder.CreateZExt(
        builder.CreateICmpSLT(left, right), builder.getInt16Ty()));
  case cparse::BinaryExpression::LessEqual:
    return static_cast<llvm::Value *>(builder.CreateZExt(
        builder.CreateICmpSLE(left, right), builder.getInt16Ty()));
  case cparse::BinaryExpression::Greater:
    return static_cast<llvm::Value *>(builder.CreateZExt(
        builder.CreateICmpSGT(left, right), builder.getInt16Ty()));
  case cparse::BinaryExpression::GreaterEqual:
    return static_cast<llvm::Value *>(builder.CreateZExt(
        builder.CreateICmpSGE(left, right), builder.getInt16Ty()));
  default:
    throw std::runtime_error("Error: unknown binary operator");
  }
}

std::any IREmitter::visit(cparse::AssignmentExpression &node) {
  auto lhs_expr = dynamic_cast<cparse::IdentifierExpression *>(node.left.get());
  if (!lhs_expr) {
    throw std::runtime_error(
        "Error: left-hand side of assignment must be a variable");
  }

  auto symbol = getSymbol(lhs_expr->name);
  if (!symbol) {
    throw std::runtime_error("Error: use of undeclared variable " +
                             lhs_expr->name);
  }

  auto right = std::any_cast<llvm::Value *>(visit(*node.right));
  builder.CreateStore(right, symbol->value);

  return static_cast<llvm::Value *>(right);
}

std::any IREmitter::visit(cparse::ConditionalExpression &node) {
  if (!node.condition || !node.then_expr || !node.else_expr) {
    throw std::runtime_error("Error: incomplete conditional expression");
  }

  auto cond_val = std::any_cast<llvm::Value *>(visit(*node.condition));
  cond_val = builder.CreateICmpNE(
      cond_val, llvm::ConstantInt::get(builder.getInt16Ty(), 0));

  auto function = builder.GetInsertBlock()->getParent();

  auto then_bb = llvm::BasicBlock::Create(context, "", function);
  auto else_bb = llvm::BasicBlock::Create(context, "", function);
  auto merge_bb = llvm::BasicBlock::Create(context, "", function);

  builder.CreateCondBr(cond_val, then_bb, else_bb);

  builder.SetInsertPoint(then_bb);
  auto then_val = std::any_cast<llvm::Value *>(visit(*node.then_expr));
  builder.CreateBr(merge_bb);
  then_bb = builder.GetInsertBlock();

  builder.SetInsertPoint(else_bb);
  auto else_val = std::any_cast<llvm::Value *>(visit(*node.else_expr));
  builder.CreateBr(merge_bb);
  else_bb = builder.GetInsertBlock();
  builder.SetInsertPoint(merge_bb);
  auto phi = builder.CreatePHI(builder.getInt16Ty(), 2);
  phi->addIncoming(then_val, then_bb);
  phi->addIncoming(else_val, else_bb);

  return static_cast<llvm::Value *>(phi);
}
