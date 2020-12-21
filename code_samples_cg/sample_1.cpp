private static List<String> computeOrders() {
        updateState();
        System.err.println("currentState : " + currentState);
        List<String> orders = new ArrayList<>();
        if (currentState == State.SUMMON_HULK) {
            orders.add(HeroType.HULK.name());
        } else if (currentState == State.SUMMON_DEADPOOL) {
            orders.add(HeroType.DEADPOOL.name());
        } else if (currentState == State.BUY_ITEMS) {
            orders.addAll(computeBuyItems());
        } else if (currentState == State.EARLY_POSITIONING) {
            orders.addAll(computeEarlyPositioning());
        } else if (currentState == State.PUSH) {
            orders.addAll(computePush());
        } else if (currentState == State.ENGAGE) {
            orders.addAll(computeEngage());
        } else if (currentState == State.KILL) {
            orders.addAll(computeKill());
        }
        return orders;
    }

    private static void updateState() {
        if (currentState == null) {
            currentState = State.SUMMON_HULK;
        } else if (currentState == State.SUMMON_HULK) {
            currentState = State.SUMMON_DEADPOOL;
        } else if (currentState == State.SUMMON_DEADPOOL) {
            currentState = State.BUY_ITEMS;
            turn = 0;
        } else if (currentState == State.BUY_ITEMS) {
            if (canNotBuyMoreStartingItems()) currentState = State.EARLY_POSITIONING;
        } else if (currentState == State.EARLY_POSITIONING) {
            if (isPositioningDone()) {
                if (canEngage()) {
                    if (opponentsAreMelee()) currentState = State.ENGAGE;
                    else currentState = State.KILL;
                } else {
                    currentState = State.PUSH;
                }
            }
        } else if (currentState == State.PUSH) {
            if (canEngage()) {
                if (opponentsAreMelee()) currentState = State.ENGAGE;
                else currentState = State.KILL;
            }
        } else if (currentState == State.ENGAGE) {
            currentState = State.KILL;
        } else if (currentState == State.KILL) {
            Hero heroToFocus = computeHeroToFocus();
            if (heroToFocus == null) currentState = State.PUSH;
        }
    }