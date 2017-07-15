local function activate()
   if(msg) then
        NPL.load("(gl)script/ide/commonlib.lua");
        commonlib.echo("===========msg");
        commonlib.echo(msg);
        if(msg.build and msg.filename)then
            NPL.load("Mod/NplCadLibrary/services/CSGService.lua");
            local CSGService = commonlib.gettable("Mod.NplCadLibrary.services.CSGService");
            local filename = msg.filename;
            local output = CSGService.build(filename,true)
            commonlib.echo("========output");
            commonlib.echo(output);
        end
            
   end
end
NPL.this(activate);