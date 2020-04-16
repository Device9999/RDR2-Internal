#pragma once
#include "../memory/memory.hpp"
typedef DWORD Void;
typedef DWORD Any;
typedef DWORD uint;
typedef DWORD Hash;
typedef int Entity;
typedef int Player;
typedef int FireId;
typedef int Ped;
typedef int Vehicle;
typedef int Cam;
typedef int CarGenerator;
typedef int Group;
typedef int Train;
typedef int Pickup;
typedef int Object;
typedef int Weapon;
typedef int Interior;
typedef int Blip;
typedef int Texture;
typedef int TextureDict;
typedef int CoverPoint;
typedef int Camera;
typedef int TaskSequence;
typedef int ColourIndex;
typedef int Sphere;
typedef int INT, ScrHandle;
struct vector_3_aligned
{
	alignas(8) float x;
	alignas(8) float y;
	alignas(8) float z;

	vector_3_aligned()
		: x(0.f)
		, y(0.f)
		, z(0.f)
	{ }

	vector_3_aligned(float x, float y, float z)
		: x(x)
		, y(y)
		, z(z)
	{ }


	bool operator!=(const vector_3_aligned& src) const {
		return (src.x != x) && (src.y != y) && (src.z != z);
	}


	bool is_valid() {
		if ((x != 0.f && y != 0.f && z != 0.f))
			return true;
		return false;
	}

	float length() {
		return sqrtf(this->x * this->x + this->y * this->y + this->z * this->z);
	}

	float dist_to(const vector_3_aligned & othervec) {
		auto delta = vector_3_aligned(this->x - othervec.x, this->y - othervec.y, this->z - othervec.z);
		return sqrtf(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
	}
};

struct Vector3
{
	Vector3() {
		x = y = z = 0.f;
	}
	Vector3(float x_, float y_, float z_) {
		x = x_;
		y = y_;
		z = z_;
	}

	bool is_valid_xy() {
		return (x != 0.f && y != 0.f);
	}

	bool is_valid_xyz() {
		return (x != 0.f && y != 0.f && z != 0.f);
	}

	float length() {
		return sqrtf(x * x + y * y + z * z);
	}

	float dist_to(const Vector3& othervec) {
		auto delta = Vector3(this->x - othervec.x, this->y - othervec.y, this->z - othervec.z);
		return sqrtf(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
	}

	bool operator!=(const Vector3& src) const {
		return (src.x != x) && (src.y != y) && (src.z != z);
	}

	Vector3 operator+(const Vector3& src) const {
		return Vector3(x + src.x, y + src.y, z + src.z);
	}


	float x;
	float y;
	float z;
};

extern "C" void* _spoofer_stub();


namespace detail
{
	template <typename Ret, typename... Args>
	static inline auto shellcode_stub_helper(
		const void* shell,
		Args... args
	) -> Ret
	{
		auto fn = (Ret(*)(Args...))(shell);
		return fn(args...);
	}

	template <std::size_t Argc, typename>
	struct argument_remapper
	{
		// At least 5 params
		template<
			typename Ret,
			typename First,
			typename Second,
			typename Third,
			typename Fourth,
			typename... Pack
		>
			static auto do_call(
				const void* shell,
				void* shell_param,
				First first,
				Second second,
				Third third,
				Fourth fourth,
				Pack... pack
			) -> Ret
		{
			return shellcode_stub_helper<
				Ret,
				First,
				Second,
				Third,
				Fourth,
				void*,
				void*,
				Pack...
			>(
				shell,
				first,
				second,
				third,
				fourth,
				shell_param,
				nullptr,
				pack...
				);
		}
	};

	template <std::size_t Argc>
	struct argument_remapper<Argc, std::enable_if_t<Argc <= 4>>
	{
		// 4 or less params
		template<
			typename Ret,
			typename First = void*,
			typename Second = void*,
			typename Third = void*,
			typename Fourth = void*
		>
			static auto do_call(
				const void* shell,
				void* shell_param,
				First first = First{},
				Second second = Second{},
				Third third = Third{},
				Fourth fourth = Fourth{}
			) -> Ret
		{
			return shellcode_stub_helper<
				Ret,
				First,
				Second,
				Third,
				Fourth,
				void*,
				void*
			>(
				shell,
				first,
				second,
				third,
				fourth,
				shell_param,
				nullptr
				);
		}
	};
}


template <typename Ret, typename... Args>
static inline auto spoof_call(
	const void* trampoline,
	Ret(*fn)(Args... args),
	Args... args
) -> Ret
{
	struct shell_params
	{
		const void* trampoline;
		void* function;
		void* rbx;
	};

	shell_params p{ trampoline, reinterpret_cast<void*>(fn) };
	using mapper = detail::argument_remapper<sizeof...(Args), void>;
	return mapper::template do_call<Ret, Args...>((const void*)& _spoofer_stub, &p, args...);
}

class Context // credits to rdr2 ScriptHook
{
	// Internal RAGE stuff
	uint64_t* retVal = stack;
	uint64_t argCount = 0;
	uint64_t* stackPtr = stack;
	uint64_t dataCount = 0;
	uint64_t spaceForResults[24];
	// Our stack
	uint64_t stack[24]{ 0 };

public:
	template<class T>
	T& At(uint32_t idx) {
		static_assert(sizeof(T) <= 8, "Argument is too big");

		return *reinterpret_cast<T*>(stack + idx);
	}

	uint32_t GetArgsCount() {
		return argCount;
	}

	void SetArgsCount(uint32_t idx) {
		argCount = idx;
	}

	template<class T, class... Args>
	void Push(T arg, Args... args) {
		static_assert(sizeof(T) <= 8, "Argument is too big");

		*(T*)(stack + argCount++) = arg;

		if constexpr (sizeof...(Args) > 0)
			Push(args...);
	}

	template<class T>
	T Result() {
		return *reinterpret_cast<T*>(retVal);
	}
	template<>
	void Result<void>() { }

	template<>
	Vector3 Result<Vector3>() {
		Vector3 vec;
		vec.x = *(float*)((uintptr_t)retVal + 0);
		vec.y = *(float*)((uintptr_t)retVal + 8);
		vec.z = *(float*)((uintptr_t)retVal + 16);
		return vec;
	}

	void Reset() {
		argCount = 0;
		dataCount = 0;
	}

	void CopyResults() {
		uint64_t a1 = (uint64_t)this;

		uint64_t result;

		for (; *(uint32_t*)(a1 + 24); *(uint32_t*)(*(uint64_t*)(a1 + 8i64 * *(signed int*)(a1 + 24) + 32) + 16i64) = result)
		{
			--* (uint32_t*)(a1 + 24);
			**(uint32_t * *)(a1 + 8i64 * *(signed int*)(a1 + 24) + 32) = *(uint32_t*)(a1 + 16 * (*(signed int*)(a1 + 24) + 4i64));
			*(uint32_t*)(*(uint64_t*)(a1 + 8i64 * *(signed int*)(a1 + 24) + 32) + 8i64) = *(uint32_t*)(a1
				+ 16i64
				* *(signed int*)(a1 + 24)
				+ 68);
			result = *(unsigned int*)(a1 + 16i64 * *(signed int*)(a1 + 24) + 72);
		}
		-- * (uint32_t*)(a1 + 24);
	}
};

typedef void(__cdecl * Handler)(Context * context);
template<class Retn = uint64_t, class... Args>
static Retn invoke_(Handler fn, Args... args)
{
	static const void* jmp_rbx = (void*)memory::find_signature(0, "\xFF\x23", "xx");
	static Context ctx;

	if (!fn) return Retn();

	ctx.Reset();

	if constexpr (sizeof...(Args) > 0)
		ctx.Push(args...);

	spoof_call(jmp_rbx, fn, &ctx);

	return ctx.Result<Retn>();
}

static Handler get_handler(uintptr_t hash_) {
	static const void* jmp_rbx = (void*)memory::find_signature(0, "\xFF\x23", "xx");

	static std::map<uint64_t, uint64_t> hash_table = { { 0, 0 } };
	static auto get_native_address = reinterpret_cast<uintptr_t(*)(uint64_t)>((uintptr_t(GetModuleHandleW(0))) + 0x2a4fcc8);
	Handler call_function = nullptr;
	auto it = hash_table.find(hash_);
	if (it == hash_table.end()) { // shit doesn't exist on our map, init that shit
		auto address = spoof_call(jmp_rbx, get_native_address, hash_);
		if (address) {
			hash_table.insert(std::pair<uint64_t, uint64_t>(hash_, address));
			call_function = (Handler)(address);
		}
	}
	else {
		if (it->first == hash_)
			call_function = (Handler)(it->second);
	}
	return call_function;
}

template<class Retn = uint64_t, class... Args>
static Retn invoke(uint64_t hashName, Args... args) {
	return invoke_<Retn>(get_handler(hashName), args...);
}
