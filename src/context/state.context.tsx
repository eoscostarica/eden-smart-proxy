import React, { useEffect, createContext, useCallback } from 'react'

import { stateType, InitialStateType } from '../@types/context'
import useLightUAL from '../hooks/useUAL'
import { walletConfig } from 'config'

const initialValue = {
  user: null,
  openMenuWallets: false,
  elemRef: null,
  ual: null
}

const SharedStateContext = createContext<{
  state: InitialStateType
  dispatch: React.Dispatch<any>
}>({
  state: initialValue,
  dispatch: () => null
})

const sharedStateReducer = (state, action): stateType => {
  switch (action.type) {
    case 'set': {
      return {
        ...state,
        ...action.payload
      }
    }

    case 'setUser': {
      return {
        ...state,
        user: action.payload
      }
    }

    case 'ual': {
      return {
        ...state,
        ual: action.ual
      }
    }

    case 'setOpenMenuWallets': {
      return {
        ...state,
        elemRef: action.payload,
        openMenuWallets: Boolean(action.payload)
      }
    }

    default: {
      throw new Error(`Unsupported action type: ${action.type}`)
    }
  }
}

export const SharedStateProvider: React.FC = ({ children, ...props }: any) => {
  const ualState = useLightUAL({
    appName: walletConfig.appName,
    chains: walletConfig.network,
    authenticators: walletConfig.authenticators
  })
  const [state, dispatch] = React.useReducer(sharedStateReducer, {
    ...initialValue
  })
  const value = React.useMemo(() => [state, dispatch], [state])

  const loadData = useCallback(async ualState => {
    dispatch({ type: 'ual', ual: ualState })
  }, [])

  useEffect(() => {
    if (!ualState) return

    if (!state?.ual || (!state?.ual.activeUser && ualState?.activeUser))
      loadData(ualState)
  }, [ualState, loadData, state])

  return (
    <SharedStateContext.Provider value={value} {...props}>
      {children}
    </SharedStateContext.Provider>
  )
}

export const useSharedState = (): any => {
  const context = React.useContext(SharedStateContext)

  if (!context) {
    throw new Error(`useSharedState must be used within a SharedStateContext`)
  }

  const stateTemp = context[0]
  const dispatchTemp = context[1]

  const setState = (payload: any) => dispatchTemp({ type: 'set', payload })
  const login = (type: string) => {
    stateTemp.ual.login(type)
  }
  const logout = () => {
    dispatchTemp({ type: 'logout' })
    stateTemp.ual.logout()
  }
  const handleOpenMenu = (event: any) => {
    dispatchTemp({
      type: 'setOpenMenuWallets',
      payload: event.currentTarget
    })
  }
  const handleCloseMenu = () => {
    dispatchTemp({
      type: 'setOpenMenuWallets',
      payload: null
    })
  }

  return [
    stateTemp,
    {
      setState,
      login,
      logout,
      handleOpenMenu,
      handleCloseMenu
    }
  ]
}
