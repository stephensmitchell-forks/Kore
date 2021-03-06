#pragma once

#include <Kore/Graphics4/VertexStructure.h>

namespace Kore {
	namespace Graphics4 {
		class VertexBuffer;
	}

	class VertexBufferImpl {
	protected:
		VertexBufferImpl(int count, int instanceDataStepRate);
		void unset();
		float* data;
		int myCount;
		int myStride;
		uint bufferId;
		uint usage;
		int sectionStart;
		int sectionSize;
		//#if defined KORE_ANDROID || defined KORE_HTML5 || defined KORE_TIZEN
		Graphics4::VertexStructure structure;
		//#endif
		int instanceDataStepRate;
		int setVertexAttributes(int offset);
#ifndef NDEBUG
		bool initialized;
#endif
	public:
		static Graphics4::VertexBuffer* current;
	};
}
