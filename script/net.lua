--[[
@ lurpc - Simple and light rpc framework.
@
@ Author luhengsi 
@ Mail   luhengsi@163.com
@ Date   2018-05-11
@
@ please check Licence.txt file for licence and legal issues. 
]]

-- require("rpc");
rpc = {}
rpc._net_ = rpc._net_ or {};
local net = rpc._net_;

function net.syn_call(cmd, ...)
    -- to do 
	net.fun_2()
    local var = glib.rpc_call(cmd, ...);
    return var; --"hello world!", 100;
end

function net.fun_1()
	local tb = {};
	-- print(debug.traceback("Stack trace"))
	-- print(debug.getinfo(1))
	local a = 1 + tb.c;
end


function net.fun_2()
	net.fun_1()
end

