#include "TextRenderer.hpp"

#include "../ResourceManager.hpp"
#include "../Console.hpp"

TextRenderer::TextRenderer()
{
  shader = ResourceManager::GetShader("base");
  model = ResourceManager::GetModel("quad");
}

void TextRenderer::draw()
{
}
