#include <vector>

class Object
{
	// ...
};


class ObjectPool
{
	private:
		struct PoolRecord
		{
			Object* instance;
			bool    in_use;
		};

		std::vector<PoolRecord> m_pool;

	public:
		Object* createNewObject()
		{
			for (size_t i = 0; i < m_pool.size(); ++i)
			{
				if (! m_pool[i].in_use)
				{
					m_pool[i].in_use = true; // переводим объект в список используемых
					return m_pool[i].instance;
				}
			}
			// если не нашли свободный объект, то расширяем пул
			PoolRecord record;
			record.instance = new Object;
			record.in_use   = true;

			m_pool.push_back(record);

			return record.instance;
		}

		void deleteObject(Object* object)
		{
			// в реальности не удаляем, а лишь помечаем, что объект свободен
			for (size_t i = 0; i < m_pool.size(); ++i)
			{
				if (m_pool[i].instance == object)
				{
					m_pool[i].in_use = false;
					break;
				}
			}
		}

		virtual ~ObjectPool()
		{
			// теперь уже "по-настоящему" удаляем объекты
			for (size_t i = 0; i < m_pool.size(); ++i)
				delete m_pool[i].instance;
		}
};


int main() {
	ObjectPool pool;
	for (size_t i = 0; i < 1000; ++i)
	{
		Object* object = pool.createNewObject();
		pool.deleteObject(object);
	}
	return 0;
}