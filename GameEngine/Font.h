#pragma once
#include <string>

struct TTF_Font;
namespace ge
{
	/**
	 * Simple RAII wrapper for a TTF_Font
	 */
	class Font final
	{
	public:
		TTF_Font* GetFont() const;
		explicit Font(const std::string& fullPath, float size);
		~Font();

		Font(const Font &) = delete;
		Font(Font &&) = delete;
		Font & operator= (const Font &) = delete;
		Font & operator= (const Font &&) = delete;

		void SetFontSize(float newSize);

	private:
		TTF_Font* m_Font;
		const std::string m_FontPath;
	};
}
