local Database = {}
Database.__index = Database
local hive : NsRemembery__Hive = {}
Database.Hive = hive

local Execute = Remembery.Execute
local Step = Remembery.Step

function Database:Lines(sql : string, cacheCount : integer)
	if cacheCount == nil then
		cacheCount = -1
	end

	local honey = Execute(self.Hive, sql, nil)
	local cache : { integer : any } = {}

	return function (t, k : integer?)
		if k == nil or k > #cache then
			-- reinit
			cache = Step(honey, cacheCount)
			k = nil
		end

		-- tail call to next
		return next(cache, k)
	end
end

function Database:Commit()

end

function Database.New(path)
	local handle : NsRemembery__Hive? = Remembery.CreateDatabase(path, "", "")
	local object = { Handle = handle }
	return setmetatable(object, Database)
end

return Database