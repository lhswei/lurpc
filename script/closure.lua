--[[
@ lurpc - Simple and light rpc framework.
@
@ Author luhengsi 
@ Mail   luhengsi@163.com
@ Date   2018-05-11
@
@ please check Licence.txt file for licence and legal issues. 
]]

require("net");

rpc._closuer_ = rpc._closuer_ or {};
local cl = rpc._closuer_;
local net = rpc._net_;

function cl.rpc_call(cmd, ...)
    -- body
    return net.syn_call(cmd, ...);
end

function cl.make_closure(cmd)
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
        return cl.rpc_call(cmd, ...);
    end

    if tb ~= rpc then
        setmetatable(tb, mt);
        return 1;
    end
    return 0;
end

-- use
cl.make_closure("online.childrenday.dailyaward");
-- local msg, code = rpc.online.childrenday.dailyaward("hi!");
-- print(msg, code)
rpc._closuer_.make_closure("online.childrenday.weeklyaward");

