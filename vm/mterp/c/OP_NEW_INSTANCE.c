HANDLE_OPCODE(OP_NEW_INSTANCE /*vAA, class@BBBB*/)
    {
        ClassObject* clazz;
        Object* newObj;

        EXPORT_PC();

        vdst = INST_AA(inst);
        ref = FETCH(1);
        ILOGV("|new-instance v%d,class@0x%04x", vdst, ref);
        clazz = dvmDexGetResolvedClass(methodClassDex, ref);
        if (clazz == NULL) {
            clazz = dvmResolveClass(curMethod->clazz, ref, false);
            if (clazz == NULL)
                GOTO_exceptionThrown();
        }

        if (!dvmIsClassInitialized(clazz) && !dvmInitClass(clazz))
            GOTO_exceptionThrown();

        /*
         * Verifier now tests for interface/abstract class.
         */
        //if (dvmIsInterfaceClass(clazz) || dvmIsAbstractClass(clazz)) {
        //    dvmThrowExceptionWithClassMessage("Ljava/lang/InstantiationError;",
        //        clazz->descriptor);
        //    GOTO_exceptionThrown();
        //}
        newObj = dvmAllocObject(clazz, ALLOC_DONT_TRACK);
        if (newObj == NULL)
            GOTO_exceptionThrown();
        SET_REGISTER(vdst, (u4) newObj);
    }
    FINISH(2);
OP_END
