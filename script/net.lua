--[[
@ lurpc - Simple and light rpc framework.
@
@ Author luhengsi 
@ Mail   luhengsi@163.com
@ Date   2018-05-11
@
@ please check Licence.txt file for licence and legal issues. 
]]

require("rpc");
rpc._net_ = rpc._net_ or {};
local net = rpc._net_;

function net.syn_call(cmd, ...)
    -- to do 
    local var = glib.rpc_call(cmd, ...);
    return var; --"hello world!", 100;
end
