def script_cmdline(scripter, tool_run_inf):
    inputs = tool_run_inf.target.get_actual_inputs()
    command_line = ((
        "%s -O coff -I %s %s -o %s"
        ) %
    (scripter.get_cross_build_tool('windres', tool_run_inf),
     inputs[0].pathname,
     inputs[1].pathname,
     tool_run_inf.target.pathname,))
    return command_line

targets['console.res'] = Target(
    tool = Script(script_cmdline),
    inputs = [
       Input('.'),
       Input('console.rc'),
    ],
)
