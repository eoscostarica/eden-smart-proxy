import { sdkConfig } from 'config'

export const getBpJons = async (owners: Array<string>): Promise<Array<any>> => {
  try {
    const {
      getProducersInfo: { producersInfo }
    } = await (
      await fetch(sdkConfig.producersInfoApiUrl, {
        method: 'POST',
        headers: {
          Accept: 'application/json',
          'Content-Type': 'application/json'
        },
        body: JSON.stringify({ bpParams: { owners } })
      })
    ).json()

    return Object?.values(producersInfo[0])?.slice(0, -1)
  } catch (error) {
    return []
  }
}
