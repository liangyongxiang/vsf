#@vsh
Feature: vsh: vsf shell
    TODO: Add more infomation

    #Scenario: vsh
    #    Given connect vsh
    #    When type vsh
    #    Then receive string

    Scenario: ls
        Given connect vsh
        When type <vsh_input>
        Then type <vsh_input>, then <vsh_output> in output

        Examples:
        | vsh_input                         | vsh_output            |
        | "ls /"                            | "bin.*"               |
        | "ls /bin"                         | "sh.*"                |
        | "free"                            | "Mem:.*"              |
        | "echo hello"                      | "hello"               |
        | "cat /fatfs/FAKEFAT32/readme.txt" | "readme.*"            |
        | "time ls /"                       | "take.*"              |
        | "export"                          | "PATH.*"              |
        #| "coremark"            | "CoreMark Size.*"     |
