--[[
@ lurpc - Simple and light rpc framework.
@
@ Copyright (c) 2018 luhengsi (luhengsi@163.com)
@ 
@ please check Licence.txt file for licence and legal issues. 
]]

rpc = rpc or {}
function rpc_call(cmd, ...)
    -- body
    return "hello world!", 100
end

function make_closure(cmd)
    -- body
    if type(cmd) ~= "string" or cmd == "" then
        return;
    end
    
    local tb = rpc
    for w in string.gmatch(cmd, "%a+") do
        -- print(w);
        tb[w] = tb[w] or {};
        tb = tb[w];
    end

    local mt = {}
    mt.__call = function(tb, ...)
        -- body
        return rpc_call(cmd, ...);
    end

    if tb ~= rpc then
        setmetatable(tb, mt);
        return 1;
    end
    return 0;
end

-- use
-- make_closure("online.childrenday.dailyaward");
-- local msg, code = rpc.online.childrenday.dailyaward("hi!");
-- print(msg, code)



