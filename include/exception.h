#ifndef RAFT_EXCEPTION_H
#define RAFT_EXCEPTION_H

namespace Raft {
	class exception {
	protected:
		const std::string variant = "";
		std::string detail = "";
	public:
		exception() {}
		exception(const exception &ec) : variant(ec.variant), detail(ec.detail) {}
		virtual std::string what() {
			return variant + " " + detail;
		}
	};

	class empty_logEntries : public exception {
		/* __________________________ */
	};
}

#endif
